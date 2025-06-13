#ifndef SYSMEM_H
#define SYSMEM_H

#include <psp2/kernel/clib.h>
#include <psp2/types.h>

typedef struct {
    SceClibMspace   mspace;
    SceVoid*        memBlock;
    SceUInt         size;
} Heap;

SceVoid*        allocMemBlockUserRw(SceName, SceUInt);
SceVoid*        allocMemBlockUserRwNc(SceName, SceUInt);
SceVoid*        allocMemBlockUserPhyContRw(SceName, SceUInt);
SceVoid*        allocMemBlockUserPhyContRwNc(SceName, SceUInt);
SceVoid*        allocMemBlockUserCdramRw(SceName, SceUInt);
SceInt          freeMemBlock(SceVoid*);
SceClibMspace   createMspace(SceVoid*, SceUInt);
SceInt          destroyMspace(SceClibMspace);

#endif

