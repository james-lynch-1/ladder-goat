#include "component.h"

const TaskData gTaskTable[NUM_TASK_TYPES] = {
    {taskMoveForward, 16, TASK_MVMT_FLAG},
    {taskMoveBackward, 16, TASK_MVMT_FLAG},
    {taskTurnLeft, 16, TASK_TURN_FLAG},
    {taskTurnRight, 16, TASK_TURN_FLAG},
};

TaskQueueComponent* addComponentTaskQueue(int entId, int flags) {
    TaskQueueComponent t = { {entId, flags}, {}, 0, 0 };
    return (TaskQueueComponent*)addComponentCustom(&t, COMP_TASK_QUEUE);
}

void removeComponentTaskQueue(int entId) {
    removeComponent(entId, COMP_TASK_QUEUE);
}

void updateTaskQueues() {
    for (int i = 0; i < numComps(COMP_TASK_QUEUE); i++) {
        TaskQueueComponent* tQ = &gTaskQueueCompsDense[i];
        if (isTaskQueueEmpty(tQ)) continue;
        Task* currTask = &tQ->queue[tQ->head];
        gTaskTable[currTask->taskIndex].fn(currTask);
        if (--currTask->timeRemaining <= 0) {
            if (tQ->head != tQ->tail)
                tQ->head = (tQ->head + 1) % (sizeof(tQ->queue) / sizeof(Task));
        }
    }
}

bool addTaskToQueue(int entId, int taskIndex, int length) {
    TaskQueueComponent* tQ = getComponent(entId, COMP_TASK_QUEUE);
    if (!tQ || isTaskQueueFull(tQ)) return false;
    Task t = { taskIndex, length > 0 ? length : gTaskTable[taskIndex].length };
    tQ->queue[tQ->tail] = t;
    tQ->tail = (tQ->tail + 1) % (sizeof(tQ->queue) / sizeof(Task));
    return true;
}

bool isTaskWithFlagsRunning(int taskFlags, int entId) {
    TaskQueueComponent* tQ = getComponent(entId, COMP_TASK_QUEUE);
    Task* currTask = &tQ->queue[tQ->head];
    if (!tQ) return true;
    if (isTaskQueueEmpty(tQ)) return false;
    return (taskFlags & gTaskTable[currTask->taskIndex].flags) != 0;
}

bool isTaskWithFlagsInQueue(int taskFlags, int entId, bool includeRunningTask) {
    TaskQueueComponent* tQ = getComponent(entId, COMP_TASK_QUEUE);
    if (!tQ) return true;
    if (isTaskQueueEmpty(tQ)) return false;
    int i = tQ->head + !includeRunningTask;
    while (i != tQ->tail) {
        if (taskFlags & gTaskTable[tQ->queue[i].taskIndex].flags)
            return true;
        i = (i + 1) % (sizeof(tQ->queue) / sizeof(Task));
    }
    return false;
}

bool isTaskQueueEmpty(TaskQueueComponent* tQ) {
    return tQ->head == tQ->tail;
}

bool isTaskQueueFull(TaskQueueComponent* tQ) {
    return tQ->head == (tQ->tail + 1) % (sizeof(tQ->queue) / sizeof(Task));
}
