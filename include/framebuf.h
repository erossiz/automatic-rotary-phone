#ifndef FRAMEBUF_H
#define FRAMEBUF_H

#include <psp2/display.h>
#include <psp2/kernel/clib.h>
#include <psp2/types.h>

typedef struct {
    SceDisplayFrameBuf  **bufs;
    SceUInt             swaps;
    SceUInt             bufMemSize;
    SceInt              frontIdx;
} FrameBufPool;

SceDisplayFrameBuf*     initFrameBuf(SceClibMspace);
SceInt                  freeFrameBuf(SceClibMspace, SceDisplayFrameBuf*);
FrameBufPool*           initFrameBufPool(SceClibMspace, SceUInt);
SceInt                  freeFrameBufPool(SceClibMspace, FrameBufPool*);

#endif

