#include "taskListGlobal.h"

void updateTaskListGlobal() {
    for (int i = 0; i < gNumTasksGlobal; i++) {
        gTasksGlobal[i].fn(gTasksGlobal[i].entId, gTasksGlobal[i].data);
        if (gTasksGlobal[i].timeRemaining != INT32_MAX) {
            gTasksGlobal[i].timeRemaining--;
            if (gTasksGlobal[i].timeRemaining <= 0) {
                gTasksGlobal[i] = gTasksGlobal[--gNumTasksGlobal];
                i--;
            }
        }
    }
}

bool addTaskGlobal(void (*fn)(int entId, int data), int timeRemaining, int entId, int data) {
    if (gNumTasksGlobal >= MAX_TASKS_GLOBAL) return false;
    TaskGlobal new = { .fn = fn, .timeRemaining = timeRemaining, .entId = entId, .data = data };
    gTasksGlobal[gNumTasksGlobal++] = new;
    return true;
}

bool isTaskGlobalInList(void (*fn)(int entId, int data), int data) {
    for (int i = 0; i < gNumTasksGlobal; i++)
        if (gTasksGlobal[i].fn == fn && gTasksGlobal[i].data == data) return true;
    return false;
}
