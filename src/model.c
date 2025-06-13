#include <errno.h>
#include <model.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/clib.h>
#include <psp2/types.h>
#include <sysmem.h>
#include <vecmath.h>

Model* initModel(SceClibMspace mspace, SceName file) {
    if (!mspace || !file) {
        errno = ERR_NULL_POINTER;
        return NULL;
    }

    SceUID uid = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (uid < 0) {
        errno = ERR_FILE_OPEN; 
        return NULL;
    }

    Model *model = (Model*) sceClibMspaceMalloc(mspace, sizeof(Model));
    if (!model) {
        errno = ERR_MSPACE_MALLOC;
        sceIoClose(uid);
        return NULL;
    }

    *model = (Model) {0};
    mat4Identity(&model->mat);

    SceInt ret = sceIoRead(uid, &model->vertsNum, sizeof(SceInt));
    if (ret <= 0) {
        errno = ERR_FILE_READ;
        ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        ret = sceIoClose(uid);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    ret = sceIoRead(uid, &model->facesNum, sizeof(SceInt));
    if (ret <= 0) {
        errno = ERR_FILE_READ;
        ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        ret = sceIoClose(uid);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    model->verts = (SceFVector3*) sceClibMspaceMalloc(mspace, sizeof(SceFVector3) * model->vertsNum);
    if (!model->verts) {
        errno = ERR_MSPACE_MALLOC;
        ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    ret = sceIoRead(uid, model->verts, sizeof(SceFVector3) * model->vertsNum);
    if (ret <= 0) {
        errno = ERR_FILE_READ;
        SceInt ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        ret = sceIoClose(uid);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    model->faces = (SceIVector3*) sceClibMspaceMalloc(mspace, sizeof(SceIVector3) * model->facesNum);
    if (!model->faces) {
        errno = ERR_MSPACE_MALLOC;
        ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    ret = sceIoRead(uid, model->faces, sizeof(SceIVector3) * model->facesNum);
    if (ret <= 0) {
        errno = ERR_FILE_READ;
        SceInt ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        ret = sceIoClose(uid);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    ret = sceIoClose(uid);
    if (ret != SCE_OK) {
        errno = ERR_FILE_CLOSE;
        ret = freeModel(model, mspace);
        if (ret != SCE_OK) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    return model;
}

SceInt freeModel(SceClibMspace mspace, Model *model) {
    if (!mspace || !model || !model->verts || !model->faces) {
        return ERR_NULL_POINTER;
    }

    sceClibMspaceFree(mspace, model->verts);
    model->verts = NULL;

    sceClibMspaceFree(mspace, model->faces);
    model->faces = NULL;

    sceClibMspaceFree(mspace, model);
    model = NULL;

    return SCE_OK;
}

SceVoid translateModel(Model *model, SceFloat x, SceFloat y, SceFloat z) {
    SceFMatrix4 translationMat, modelMat;
    mat4Identity(&translationMat);
    mat4Translate(&translationMat, x, y, z);
    mat4Multiply(&modelMat, &model->mat, &translationMat);
    model->mat = modelMat;
}

SceVoid rotateModel(Model *model, SceFloat angle, SceFloat x, SceFloat y, SceFloat z) {
    SceFMatrix4 rotationMat, modelMat;
    mat4Identity(&rotationMat);
    mat4Rotate(&rotationMat, angle, x, y, z);
    mat4Multiply(&modelMat, &model->mat, &rotationMat);
    model->mat = modelMat;
}

SceVoid scaleModel(Model *model, SceFloat x, SceFloat y, SceFloat z) {
    SceFMatrix4 scaleMat, modelMat;
    mat4Identity(&scaleMat);
    mat4Scale(&scaleMat, x, y, z);
    mat4Multiply(&modelMat, &model->mat, &scaleMat);
    model->mat = modelMat;
}

