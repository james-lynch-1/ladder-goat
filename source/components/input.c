#include "component.h"

void addComponentInput(int entId, int flags, void (*inputHandler)(int entId)) {
    InputComponent inputComp = { {entId, flags}, inputHandler };
    addComponentCustom((void*)&inputComp, COMP_INPUT);
}

void handleInputPlayer(int entId) {

    if (key_is_down(KEY_UP | KEY_DOWN)) {
        if (!isTaskWithFlagsInQueue(TASK_MVMT_FLAG, entId, true)) {
            addTaskToQueue(entId, TASK_MOVE, key_is_down(KEY_UP) ? 1 : -1);
            notify(gPlayerId, COMP_SLAPPABLE, E_MOVETASK_QUEUED);
        }
    }
}

void handleInputLadder(int entId) {

    if (key_is_down(KEY_LEFT | KEY_RIGHT)) {
        if (!isTaskWithFlagsInQueue(TASK_TURN_FLAG, gPlayerId, true)) {
            addTaskToQueue(gPlayerId, TASK_TURN, key_is_down(KEY_LEFT) ? 1 : -1);
            notify(gPlayerId, COMP_SLAPPABLE, E_MOVETASK_QUEUED);
        }
    }

    if (key_hit(KEY_START)) {
        reset();
        return;
    }

    if (key_hit(KEY_SELECT)) {
        changeLevel(!gLevelData->levelId);
    }
}

void handleInputGameover(int entId) {
    if (numComps(COMP_TIMER) == 0 && key_hit(KEY_START)) {
        setGameState(NORMAL);
    }
}

void updateInputComps() {
    for (int i = 0; i < gNumCompsPerType[COMP_INPUT]; i++) {
        gInputCompsDense[i].inputHandler(gInputCompsDense[i].header.entId);
    }
}

void removeComponentInput(int entId) {
    removeComponent(entId, COMP_INPUT);
}
