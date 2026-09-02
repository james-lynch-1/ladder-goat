#include "component.h"

void addComponentInput(int entId, int flags, void (*inputHandler)(int entId)) {
    InputComponent inputComp = { {entId, flags}, inputHandler };
    addComponentCustom((void*)&inputComp, COMP_INPUT);
}

void handleInputPlayer(int entId) {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    playerPhys->pos.y.HALF.HI += key_tri_fire();
    playerPhys->pos.y.HALF.HI = clamp(playerPhys->pos.y.HALF.HI, 0, 64);

    if (key_is_down(KEY_UP | KEY_DOWN)) {
        if (!isTaskWithFlagsInQueue(TASK_MVMT_FLAG, entId, true))
            addTaskToQueue(entId, TASK_MOVE, key_is_down(KEY_UP) ? 1 : 0);
    }
}

void handleInputLadder(int entId) {
    PhysicsComponent* ladderPhys = getComponent(entId, COMP_PHYSICS);

    if (key_hit(KEY_LEFT | KEY_RIGHT)) {
        if (!isTaskWithFlagsInQueue(TASK_TURN_FLAG, gPlayerId, false))
            addTaskToQueue(gPlayerId, TASK_TURN, key_is_down(KEY_LEFT) ? 1 : 0);
    }
    else if (key_is_down(KEY_LEFT | KEY_RIGHT)) {
        if (!isTaskWithFlagsInQueue(TASK_TURN_FLAG, gPlayerId, true))
            addTaskToQueue(gPlayerId, TASK_TURN, key_is_down(KEY_LEFT) ? 1 : 0);
    }

    ladderPhys->pos.y.HALF.HI += key_tri_fire();
    ladderPhys->pos.y.HALF.HI = clamp(ladderPhys->pos.y.HALF.HI, 0, 64);

    if (key_hit(KEY_START)) { // reset
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
