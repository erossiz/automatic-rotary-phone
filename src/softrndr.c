#include <errno.h>
#include <model.h>
#include <psp2/display.h>
#include <psp2/kernel/clib.h>
#include <psp2/types.h>
#include <softrndr.h>
#include <vecmath.h>

Camera* initCamera(SceClibMspace mspace, SceFloat fov, SceFloat ratio, SceFloat near, SceFloat far) {
    if (!mspace) {
        errno = ERR_NULL_POINTER;
        return NULL;
    }

    Camera *camera = (Camera*) sceClibMspaceMalloc(mspace, sizeof(Camera));
    if (!camera) {
        errno = ERR_MSPACE_MALLOC;
        return NULL;
    }

    *camera = (Camera) {0};
    camera->pos     = (SceFVector4) {.0f, .0f, .0f, .0f};
    camera->at      = (SceFVector4) {.0f, .0f, .0f, 1.f};
    camera->up      = (SceFVector4) {.0f, 1.f, .0f, .0f};
    camera->fov     = fov;
    camera->ratio   = ratio;
    camera->near    = near;
    camera->far     = far;

    mat4View(&camera->view, &camera->pos, &camera->at, &camera->up);
    mat4Perspective(&camera->proj, camera->fov, camera->ratio, camera->near, camera->far); 

    return camera;
}

SceInt freeCamera(SceClibMspace mspace, Camera *camera) {
    if (!mspace || !camera) {
        return ERR_NULL_POINTER;
    }

    sceClibMspaceFree(mspace, camera);
    camera = NULL;

    return SCE_OK;
}

SceVoid renderModelWireframe(SceDisplayFrameBuf *frameBuf, Model *model, Camera *camera) {
    SceFMatrix4 vpMat, mvpMat;
    mat4Multiply(&vpMat, &camera->proj, &camera->view);
    mat4Multiply(&mvpMat, &vpMat, &model->mat);

    SceFVector4 projVerts[model->vertsNum];
    SceIVector2 verts2d[model->vertsNum];
    SceUInt halfWidth = frameBuf->width / 2;
    SceUInt halfHeight = frameBuf->height / 2;

    for (SceUInt i = 0; i < model->vertsNum; i++) {
        SceFVector4 vec;
        vec.x = model->verts[i].x;
        vec.y = model->verts[i].y;
        vec.z = model->verts[i].z;
        vec.w = 1.f;

        mat4MultiplyVec(&projVerts[i], &mvpMat, &vec);
        SceFloat iz = projVerts[i].z != .0f ? 1.f / projVerts[i].z : 1.f;
        verts2d[i].x = (SceInt) ((projVerts[i].x * iz) * halfWidth + halfWidth);
        verts2d[i].y = (SceInt) ((-projVerts[i].y * iz) * halfHeight + halfHeight);
    }

    for (SceUInt i = 0; i < model->facesNum; i++) {
        SceUInt v0 = model->faces[i].x;
        SceUInt v1 = model->faces[i].y;
        SceUInt v2 = model->faces[i].z;

        drawLine(frameBuf, verts2d[v0].x, verts2d[v1].x, verts2d[v0].y, verts2d[v1].y, 0xffffffff);
        drawLine(frameBuf, verts2d[v1].x, verts2d[v2].x, verts2d[v1].y, verts2d[v2].y, 0xffffffff);
        drawLine(frameBuf, verts2d[v2].x, verts2d[v0].x, verts2d[v2].y, verts2d[v0].y, 0xffffffff);
    }
}

SceVoid drawLine(SceDisplayFrameBuf *frameBuf, SceInt x0, SceInt x1, SceInt y0, SceInt y1, SceUInt color) {
    SceInt dx = x1 > x0 ? x1 - x0 : x0 - x1;
    SceInt dy = y1 > y0 ? y1 - y0 : y0 - y1;
    SceInt sx = x0 < x1 ? 1 : -1;
    SceInt sy = y0 < y1 ? 1 : -1;
    SceInt err = dx - dy;

    while (SCE_TRUE) {
        fillFragment(frameBuf, x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }

        SceInt err2 = err * 2;
        if (err2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (err2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

SceVoid fillFragment(SceDisplayFrameBuf *frameBuf, SceUInt x, SceUInt y, SceUInt color) {
    if (x < frameBuf->width && y < frameBuf->height) {
        *((SceUInt*) frameBuf->base + (y * frameBuf->pitch + x)) = color;
    }
}

