#include "component.h"

void addComponentInput(int entId, int flags, void (*inputHandler)(int entId)) {
    InputComponent inputComp = { { entId, flags }, inputHandler };
    addComponentCustom((void*)&inputComp, COMP_INPUT);
}

void handleInput() {
    if (key_is_down(KEY_FIRE | KEY_SHOULDER)) {
        gViewport.x += 2 * key_tri_shoulder();
        gViewport.y += 2 * key_tri_fire();
    }

    if (key_hit(KEY_A)) {
        PhysicsComponent* phys = getComponent(gPlayerId, COMP_PHYSICS);
        logVal("playerX: ", phys->pos.x.HALF.HI / 16);
        logVal("playerZ: ", phys->pos.z.HALF.HI / 16);
        // ObjComponent* obj = getComponent(gPlayerId, COMP_OBJ);
        // logVal("screenX: ", (obj->attr1 & ATTR1_X_MASK));
        // logVal("screenY: ", (obj->attr0 & ATTR0_Y_MASK));
    }
}

void CBInputComp(int entId) {
    if (key_hit(KEY_START)) {
        addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, gLevelData->levelId);
    }

#ifdef DEBUG
    if (key_hit(KEY_SELECT)) {
        int nextLevel = getNextLevelIndex();
        addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, nextLevel);
    }
#endif

    if (key_is_down(KEY_UP | KEY_DOWN)) {
        if (!isTaskWithFlagsInQueue(TASK_MVMT_FLAG, entId, true))
            addTaskToQueue(entId, TASK_MOVE_DISPATCH, key_is_down(KEY_UP) ? 1 : -1);
    }
    if (key_is_down(KEY_LEFT | KEY_RIGHT)) {
        bool isTurnInQueue = isTaskWithFlagsInQueue(TASK_TURN_FLAG, entId, true);
        if (!isTurnInQueue)
            addTaskToQueue(entId, TASK_TURN_DISPATCH, key_is_down(KEY_LEFT) ? 1 : -1);
    }
}

void updateInputComps() {
    for (int i = 0; i < gNumCompsPerType[COMP_INPUT]; i++) {
        if (gInputCompsDense[i].inputHandler)
            gInputCompsDense[i].inputHandler(gInputCompsDense[i].header.entId);
    }
}

void removeComponentInput(int entId) {
    removeComponent(entId, COMP_INPUT);
}
