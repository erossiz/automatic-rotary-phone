#include <math.h>
#include <psp2/types.h>
#include <vecmath.h>

SceVoid vec3Subtract(SceFVector4 *out, SceFVector4 *a, SceFVector4 *b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

SceVoid vec3Cross(SceFVector4 *out, SceFVector4 *a, SceFVector4 *b) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

SceFloat vec3Dot(SceFVector4 *a, SceFVector4 *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

SceVoid vec3Normalize(SceFVector4 *vec) {
    SceFloat len = sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    if (len > .0f) {
        vec->x /= len;
        vec->y /= len;
        vec->z /= len;
    }
}

SceVoid mat4Identity(SceFMatrix4 *mat) {
    *mat = (SceFMatrix4) {0};
    mat->x.x = 1.f;
    mat->y.y = 1.f;
    mat->z.z = 1.f;
    mat->w.w = 1.f;
}

SceVoid mat4Multiply(SceFMatrix4 *out, SceFMatrix4 *a, SceFMatrix4 *b) {
    out->x.x = a->x.x * b->x.x + a->x.y * b->y.x + a->x.z * b->z.x + a->x.w * b->w.x;
    out->x.y = a->x.x * b->x.y + a->x.y * b->y.y + a->x.z * b->z.y + a->x.w * b->w.y;
    out->x.z = a->x.x * b->x.z + a->x.y * b->y.z + a->x.z * b->z.z + a->x.w * b->w.z;
    out->x.w = a->x.x * b->x.w + a->x.y * b->y.w + a->x.z * b->z.w + a->x.w * b->w.w;

    out->y.x = a->y.x * b->x.x + a->y.y * b->y.x + a->y.z * b->z.x + a->y.w * b->w.x;
    out->y.y = a->y.x * b->x.y + a->y.y * b->y.y + a->y.z * b->z.y + a->y.w * b->w.y;
    out->y.z = a->y.x * b->x.z + a->y.y * b->y.z + a->y.z * b->z.z + a->y.w * b->w.z;
    out->y.w = a->y.x * b->x.w + a->y.y * b->y.w + a->y.z * b->z.w + a->y.w * b->w.w;

    out->z.x = a->z.x * b->x.x + a->z.y * b->y.x + a->z.z * b->z.x + a->z.w * b->w.x;
    out->z.y = a->z.x * b->x.y + a->z.y * b->y.y + a->z.z * b->z.y + a->z.w * b->w.y;
    out->z.z = a->z.x * b->x.z + a->z.y * b->y.z + a->z.z * b->z.z + a->z.w * b->w.z;
    out->z.w = a->z.x * b->x.w + a->z.y * b->y.w + a->z.z * b->z.w + a->z.w * b->w.w;

    out->w.x = a->w.x * b->x.x + a->w.y * b->y.x + a->w.z * b->z.x + a->w.w * b->w.x;
    out->w.y = a->w.x * b->x.y + a->w.y * b->y.y + a->w.z * b->z.y + a->w.w * b->w.y;
    out->w.z = a->w.x * b->x.z + a->w.y * b->y.z + a->w.z * b->z.z + a->w.w * b->w.z;
    out->w.w = a->w.x * b->x.w + a->w.y * b->y.w + a->w.z * b->z.w + a->w.w * b->w.w;
}

SceVoid mat4MultiplyVec(SceFVector4 *out, SceFMatrix4 *mat, SceFVector4 *vec) {
    out->x = mat->x.x * vec->x + mat->x.y * vec->y + mat->x.z * vec->z + mat->x.w * vec->w;
    out->y = mat->y.x * vec->x + mat->y.y * vec->y + mat->y.z * vec->z + mat->y.w * vec->w;
    out->z = mat->z.x * vec->x + mat->z.y * vec->y + mat->z.z * vec->z + mat->z.w * vec->w;
    out->w = mat->w.x * vec->x + mat->w.y * vec->y + mat->w.z * vec->z + mat->w.w * vec->w;
}

SceVoid mat4Translate(SceFMatrix4 *mat, SceFloat x, SceFloat y, SceFloat z) {
    mat4Identity(mat);
    mat->w.x = x;
    mat->w.y = y;
    mat->w.z = z;
}

SceVoid mat4Scale(SceFMatrix4 *mat, SceFloat x, SceFloat y, SceFloat z) {
    mat4Identity(mat);
    mat->x.x = x;
    mat->y.y = y;
    mat->z.z = z;   
}

SceVoid mat4Rotate(SceFMatrix4 *mat, SceFloat angle, SceFloat x, SceFloat y, SceFloat z) {
    SceFloat len = sqrtf(x * x + y * y + z * z);
    if (len == .0f) {
        mat4Identity(mat);
        return;
    }

    if (len > 1e-6f) {
        x /= len;
        y /= len;
        z /= len;
    }

    SceFloat c = cosf(angle);
    SceFloat s = sinf(angle);
    SceFloat t = 1.f - c;

    *mat = (SceFMatrix4) {0};

    mat->x.x = t * x * x + c;
    mat->x.y = t * x * y + s * z;
    mat->x.z = t * x * z - s * y;

    mat->y.x = t * x * y - s * z;
    mat->y.y = t * y * y + c;
    mat->y.z = t * y * z + s * x;

    mat->z.x = t * x * z + s * y;
    mat->z.y = t * y * z - s * x;
    mat->z.z = t * z * z + c;

    mat->w.w = 1.f;
}

SceVoid mat4View(SceFMatrix4 *view, SceFVector4 *pos, SceFVector4 *at, SceFVector4 *up) {
    SceFVector4 forward, right, reup;

    vec3Subtract(&forward, at, pos);
    vec3Normalize(&forward);

    vec3Cross(&right, &forward, up);
    vec3Normalize(&right);

    vec3Cross(&reup, &right, &forward);

    view->x.x = right.x;
    view->x.y = reup.x;
    view->x.z = -forward.x;
    view->x.w = .0f;

    view->y.x = right.y;
    view->y.y = reup.y;
    view->y.z = -forward.y;
    view->y.w = .0f;

    view->z.x = right.z;
    view->z.y = reup.z;
    view->z.z = -forward.z;
    view->z.w = .0f;

    view->w.x = -vec3Dot(&right, pos);
    view->w.y = -vec3Dot(&reup, pos);
    view->w.z = vec3Dot(&forward, pos);
    view->w.w = 1.f;
}

SceVoid mat4Perspective(SceFMatrix4 *projection, SceFloat fov, SceFloat ratio, SceFloat near, SceFloat far) {
    SceFloat scale = 1.f / tanf(fov / 2.f);
    mat4Identity(projection);
    projection->x.x = scale / ratio;
    projection->y.y = scale;
    projection->z.z = (far + near) / (near - far);
    projection->z.w = -1.f;
    projection->w.z = (2.f * far * near) / (near - far);
    projection->w.w = .0f;
}

