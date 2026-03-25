#ifndef DBG_H
#define DBG_H

#include "stm8s.h"

void Dbg_init();
void Dbg_set8(u8 code);
void Dbg_set(u8* code, u8 width);


#endif
