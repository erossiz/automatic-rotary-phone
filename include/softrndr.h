#ifndef SOFTRNDR_H
#define SOFTRNDR_H

#include <psp2/display.h>
#include <psp2/kernel/clib.h>
#include <psp2/types.h>

typedef struct {
    SceFMatrix4 view;
    SceFMatrix4 proj;
    SceFVector4 pos;
    SceFVector4 up;
    SceFVector4 at;
    SceFloat    fov;
    SceFloat    ratio;
    SceFloat    near;
    SceFloat    far;
} Camera;

Camera*     initCamera(SceClibMspace, SceFloat, SceFloat, SceFloat, SceFloat);
SceInt      freeCamera(SceClibMspace, Camera*);
SceVoid     renderModelWireframe(SceDisplayFrameBuf*, Model*, Camera*);
SceVoid     drawLine(SceDisplayFrameBuf*, SceInt, SceInt, SceInt, SceInt, SceUInt);
SceVoid     fillFragment(SceDisplayFrameBuf*, SceUInt, SceUInt, SceUInt);

#endif

