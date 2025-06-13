#ifndef MODEL_H
#define MODEL_H

#include <psp2/kernel/clib.h>
#include <psp2/types.h>

typedef struct {
    SceFMatrix4 mat;
    SceFVector3 *verts;
    SceIVector3 *faces;
    SceUInt     vertsNum;
    SceUInt     facesNum;
} Model;

Model*      initModel(SceClibMspace, SceName);
SceInt      freeModel(SceClibMspace, Model*);
SceVoid     translateModel(Model*, SceFloat, SceFloat, SceFloat);
SceVoid     rotateModel(Model*, SceFloat, SceFloat, SceFloat, SceFloat);
SceVoid     scaleModel(Model*, SceFloat, SceFloat, SceFloat);

#endif

