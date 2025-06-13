#include <errno.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/types.h>
#include <sysmem.h>

#define LPDDR_ALIGNMENT         (1024 * 4)
#define LPDDR_PHYCONT_ALIGNMENT (1024 * 1024)
#define CDRAM_ALIGNMENT         (1024 * 256)

static inline SceUInt memAlign(SceUInt size, SceUInt alignment) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

static SceVoid* allocMemBlock(SceName name, SceKernelMemBlockType type, SceUInt size) {
    SceKernelAllocMemBlockOpt opt = {0};
    opt.size = sizeof(SceKernelAllocMemBlockOpt);
    opt.attr = SCE_KERNEL_ALLOC_MEMBLOCK_ATTR_HAS_ALIGNMENT;
    switch (type) {
        case SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW:
            size = memAlign(size, CDRAM_ALIGNMENT);
            opt.alignment = CDRAM_ALIGNMENT;
            break;
        case SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW:
        case SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW:
            size = memAlign(size, LPDDR_PHYCONT_ALIGNMENT);
            opt.alignment = LPDDR_PHYCONT_ALIGNMENT;
            break;
        case SCE_KERNEL_MEMBLOCK_TYPE_USER_RW:
        case SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE:
        default:
            size = memAlign(size, LPDDR_ALIGNMENT);
            opt.alignment = LPDDR_ALIGNMENT;
            break;
    }

    SceUID uid = sceKernelAllocMemBlock(name, type, size, &opt);
    if (uid < 0) {
        errno = ERR_MEM_BLOCK_ALLOC;
        return NULL;
    }

    SceVoid *base = NULL;
    SceInt ret = sceKernelGetMemBlockBase(uid, &base);
    if (ret < 0) {
        errno = ERR_MEM_BLOCK_BASE_ADDRESS_GET;
        ret = sceKernelFreeMemBlock(uid);
        if (ret < 0) {
            errno = (errno << 8) | ret;
        }
        return NULL;
    }

    return base;
}

SceVoid* allocMemBlockUserRw(SceName name, SceUInt size) {
    return allocMemBlock(name, SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, size);
}

SceVoid* allocMemBlockUserRwNc(SceName name, SceUInt size) {
    return allocMemBlock(name, SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size);
}

SceVoid* allocMemBlockUserPhyContRw(SceName name, SceUInt size) {
    return allocMemBlock(name, SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW, size);
}

SceVoid* allocMemBlockUserPhyContRwNc(SceName name, SceUInt size) {
    return allocMemBlock(name, SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW, size);
}

SceVoid* allocMemBlockUserCdramRw(SceName name, SceUInt size) {
    return allocMemBlock(name, SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, size);
}

SceInt freeMemBlock(SceVoid *base) {
    if (!base) {
        return ERR_NULL_POINTER;
    }

    SceUID uid = sceKernelFindMemBlockByAddr(base, 0);
    if (uid < 0) {
        return ERR_MEM_BLOCK_BASE_ADDRESS_FIND;
    }

    SceInt ret = sceKernelFreeMemBlock(uid);
    if (ret < 0) {
        return ERR_MEM_BLOCK_FREE;
    }

    base = NULL;

    return SCE_OK;
}

SceClibMspace createMspace(SceVoid *base, SceUInt size) {
    if (!base) {
        errno = ERR_NULL_POINTER;
        return NULL;
    }

    SceClibMspace mspace = sceClibMspaceCreate(base, size);
    if (!mspace) {
        errno = ERR_MSPACE_CREATE;
        return NULL;
    }

    return mspace;
}

SceInt destroyMspace(SceClibMspace mspace) {
    if (!mspace) {
        return ERR_NULL_POINTER;
    }

    sceClibMspaceDestroy(mspace);
    SceInt ret = freeMemBlock(mspace);
    if (ret != SCE_OK) {
        return ret;
    }

    mspace = NULL;

    return SCE_OK;
}

