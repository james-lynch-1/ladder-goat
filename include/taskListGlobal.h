#ifndef TASKLISTGLOBAL
#define TASKLISTGLOBAL

#include "global.h"

void updateTaskListGlobal();

bool addTaskGlobal(void(*fn)(int entId, int data), int timeRemaining, int entId, int data);

bool isTaskGlobalInList(void(*fn)(int entId, int data), int data);

#endif
