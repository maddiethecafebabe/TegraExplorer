#pragma once
#include "types.h"

Vector_t runLexar(const char* in, u32 len);
char lexarDebugGetTokenC(u8 tokenN);
void lexarVectorClear(Vector_t *v);