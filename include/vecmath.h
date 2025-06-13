#ifndef VECMATH_H
#define VECMATH_H

#include <psp2/types.h>

SceVoid     vec3Subtract(SceFVector4*, SceFVector4*, SceFVector4*);
SceVoid     vec3Cross(SceFVector4*, SceFVector4*, SceFVector4*);
SceFloat    vec3Dot(SceFVector4*, SceFVector4*);
SceVoid     vec3Normalize(SceFVector4*);
SceVoid     mat4Identity(SceFMatrix4*);
SceVoid     mat4Multiply(SceFMatrix4*, SceFMatrix4*, SceFMatrix4*);
SceVoid     mat4MultiplyVec(SceFVector4*, SceFMatrix4*, SceFVector4*);
SceVoid     mat4Translate(SceFMatrix4*, SceFloat, SceFloat, SceFloat);
SceVoid     mat4Scale(SceFMatrix4*, SceFloat, SceFloat, SceFloat);
SceVoid     mat4Rotate(SceFMatrix4*, SceFloat, SceFloat, SceFloat, SceFloat);
SceVoid     mat4View(SceFMatrix4*, SceFVector4*, SceFVector4*, SceFVector4*);
SceVoid     mat4Perspective(SceFMatrix4*, SceFloat, SceFloat, SceFloat, SceFloat);

#endif

