#include <errno.h>
#include <framebuf.h>
#include <psp2/display.h>
#include <psp2/kernel/clib.h>
#include <psp2/types.h>
#include <sysmem.h>

#define FRAMEBUF_WIDTH      (960)
#define FRAMEBUF_HEIGHT     (544)
#define FRAMEBUF_PITCH      (1024)
#define FRAMEBUF_MEMSIZE    (FRAMEBUF_PITCH * FRAMEBUF_HEIGHT * 4)

SceDisplayFrameBuf* initFrameBuf(SceClibMspace mspace) {
    if (!mspace) {
        errno = ERR_NULL_POINTER;
        return NULL;
    }

    SceDisplayFrameBuf *frameBuf = (SceDisplayFrameBuf*) sceClibMspaceMalloc(mspace, sizeof(SceDisplayFrameBuf));
    if (!frameBuf) {
        errno = ERR_MSPACE_MALLOC;
        return NULL;
    }

    SceVoid *base = allocMemBlockUserCdramRw("FrameBufMem", FRAMEBUF_MEMSIZE);
    if (!base) {
        SceInt ret = freeFrameBuf(mspace, frameBuf);
        if (ret != SCE_OK) {
            errno = ret;
        }
        return NULL;
    }

    *frameBuf = (SceDisplayFrameBuf) {0};
    frameBuf->size          = sizeof(SceDisplayFrameBuf);
    frameBuf->base          = base;
    frameBuf->width         = FRAMEBUF_WIDTH;
    frameBuf->height        = FRAMEBUF_HEIGHT;
    frameBuf->pitch         = FRAMEBUF_PITCH;
    frameBuf->pixelformat   = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;

    return frameBuf;
}

SceInt freeFrameBuf(SceClibMspace mspace, SceDisplayFrameBuf *frameBuf) {
    if (!mspace || !frameBuf) {
        return ERR_NULL_POINTER;
    }

    SceInt ret = freeMemBlock(frameBuf->base);
    if (ret != SCE_OK) {
        return ret;
    }

    sceClibMspaceFree(mspace, frameBuf);
    frameBuf = NULL;

    return SCE_OK;
}

FrameBufPool* initFrameBufPool(SceClibMspace mspace, SceUInt swaps) {
    if (!mspace) {
        errno = ERR_NULL_POINTER;
        return NULL;
    }

    FrameBufPool *frameBufPool = (FrameBufPool*) sceClibMspaceMalloc(mspace, sizeof(FrameBufPool));
    if (!frameBufPool) {
        errno = ERR_MSPACE_MALLOC;
        return NULL;
    }

    *frameBufPool = (FrameBufPool) {0};
    frameBufPool->swaps      = swaps < 1 ? 1 : swaps;
    frameBufPool->frontIdx   = 0;
    frameBufPool->bufMemSize = FRAMEBUF_MEMSIZE;
    frameBufPool->bufs       = (SceDisplayFrameBuf**) sceClibMspaceMalloc(mspace, sizeof(SceDisplayFrameBuf) * swaps);
    if (!frameBufPool->bufs) {
        sceClibMspaceFree(mspace, frameBufPool);
        errno = ERR_MSPACE_MALLOC;
        return NULL;
    }

    for (SceUInt i = 0; i < swaps; i++) {
        frameBufPool->bufs[i] = initFrameBuf(mspace);
        if (!frameBufPool->bufs[i]) {
            SceInt ret = freeFrameBufPool(mspace, frameBufPool);
            if (ret != SCE_OK) {
                errno = (errno << 8) | ret;
            }
            return NULL;
        }
    }

    return frameBufPool;
}

SceInt freeFrameBufPool(SceClibMspace mspace, FrameBufPool *frameBufPool) {
    if (!mspace || !frameBufPool) {
        return ERR_NULL_POINTER;
    }

    for (SceUInt i = 0; i < frameBufPool->swaps; i++) {
        SceInt ret = freeFrameBuf(mspace, frameBufPool->bufs[i]);
        if (ret != SCE_OK) {
            return ret;
        }
    }

    sceClibMspaceFree(mspace, frameBufPool);
    frameBufPool = NULL;

    return SCE_OK;
}

