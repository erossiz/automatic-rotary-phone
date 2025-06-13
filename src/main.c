#include <framebuf.h>
#include <math.h>
#include <model.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/types.h>
#include <softrndr.h>
#include <sysmem.h>
#include <vecmath.h>

#define SYSMEM_SIZE         (1024 * 1024 * 16)
#define TARGET_FRAMETIME    (16667)
#define MODEL_FILENAME      ("app0:/assets/donut.bin")
#define CAMERA_FOV          (90.f)
#define CAMERA_SPEED        (.1f)

SceInt errno = 0;

SceInt main() {
    Heap heap = (Heap) {0};
    heap.size = SYSMEM_SIZE;
    heap.memBlock = allocMemBlockUserRw("Heap", heap.size);
    if (!heap.memBlock) {
        return sceKernelExitProcess(errno);
    }

    heap.mspace = createMspace(heap.memBlock, heap.size);
    if (!heap.mspace) {
        return sceKernelExitProcess(errno);
    }

    FrameBufPool *frameBufPool = initFrameBufPool(heap.mspace, 2);
    if (!frameBufPool) {
        return sceKernelExitProcess(errno);
    }

    Model *model = initModel(heap.mspace, MODEL_FILENAME);
    if (!model) {
        return sceKernelExitProcess(errno);
    }

    SceFloat fov = CAMERA_FOV * (M_PI / 180.f);
    SceFloat ratio = (SceFloat) frameBufPool->bufs[0]->width / (SceFloat) frameBufPool->bufs[0]->height;
    Camera *camera = initCamera(heap.mspace, fov, ratio, 0.f, 100.f);
    if (!camera) {
        return sceKernelExitProcess(errno);
    }

    camera->pos = (SceFVector4) {.0f, .0f, -20.f, .0f};
    mat4View(&camera->view, &camera->pos, &camera->at, &camera->up);

    while (SCE_TRUE) {
        SceUInt64 tick = sceKernelGetProcessTimeWide();

        SceCtrlData ctrl;
        sceCtrlPeekBufferPositive(0, &ctrl, 1);

        SceFVector4 forward;
        vec3Subtract(&forward, &camera->at, &camera->pos);
        vec3Normalize(&forward);

        if (ctrl.buttons & SCE_CTRL_UP) {
            camera->pos.x += forward.x * CAMERA_SPEED;
            camera->pos.y += forward.y * CAMERA_SPEED;
            camera->pos.z += forward.z * CAMERA_SPEED;
        } else if (ctrl.buttons & SCE_CTRL_DOWN) {
            camera->pos.x -= forward.x * CAMERA_SPEED;
            camera->pos.y -= forward.y * CAMERA_SPEED;
            camera->pos.z -= forward.z * CAMERA_SPEED;
        }

        mat4View(&camera->view, &camera->pos, &camera->at, &camera->up);

        SceDisplayFrameBuf *frontBuf = frameBufPool->bufs[frameBufPool->frontIdx];
        sceClibMemset(frontBuf->base, 0, frameBufPool->bufMemSize);

        renderModelWireframe(frontBuf, model, camera);

        sceDisplaySetFrameBuf(frontBuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
        sceDisplayWaitVblankStart();
        frameBufPool->frontIdx = (frameBufPool->frontIdx + 1) % frameBufPool->swaps;

        SceUInt64 tock = sceKernelGetProcessTimeWide();
        SceUInt64 delta = tock - tick;
        if (delta < TARGET_FRAMETIME) {
            sceKernelDelayThread(TARGET_FRAMETIME - delta);
        }
    }

    SceInt ret = freeCamera(heap.mspace, camera);
    if (ret != SCE_OK) {
        return sceKernelExitProcess(ret);
    }

    ret = freeModel(heap.mspace, model);
    if (ret != SCE_OK) {
        return sceKernelExitProcess(ret);
    }

    ret = freeFrameBufPool(heap.mspace, frameBufPool);
    if (ret != SCE_OK) {
        return sceKernelExitProcess(ret);
    }

    ret = destroyMspace(heap.mspace);
    if (ret != SCE_OK) {
        return sceKernelExitProcess(ret);
    }

    ret = freeMemBlock(heap.memBlock);
    if (ret != SCE_OK) {
        return sceKernelExitProcess(ret);
    }

    return sceKernelExitProcess(SCE_OK);
}

