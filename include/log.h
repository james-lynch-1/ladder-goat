#ifndef LOG
#define LOG

#include "type.h"
#include <tonc.h>
#include "string.h"
#include <stdlib.h>

#define VA ,(void*)&
#define log(type, val)  mgbaLog(type, (void*)&val)

#define REG_DEBUG_ENABLE (vu16 *)0x4FFF780
#define REG_DEBUG_FLAGS (vu16 *)0x4FFF700
#define REG_DEBUG_STRING (char *)0x4FFF600

void mgbaLog(enum Type type, void* messagePtr);
void logVal(char* prefix, int val);
void logPos(Position pos);
char* toHex(int num);

#endif
