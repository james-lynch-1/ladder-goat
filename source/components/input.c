#include "component.h"

void addComponentInput(int entId, int flags, void (*inputHandler)(int entId)) {
    InputComponent inputComp = { {entId, flags}, inputHandler };
    addComponentCustom((void*)&inputComp, COMP_INPUT);
}

void handleInputPlayer(int entId) {
    PhysicsComponent* physComp = getComponent(entId, COMP_PHYSICS);
    if (!physComp) return;

    if (key_is_down(KEY_UP | KEY_DOWN))
        if (isTaskQueueEmpty(getComponent(gPlayerId, COMP_TASK_QUEUE)))
            addTaskToQueue(gPlayerId, key_is_down(KEY_UP) ? TASK_MOVE_FWD : TASK_MOVE_BWD, 16);
}

void handleInputLadder(int entId) { // coupled to PhysicsComponent
    PhysicsComponent* physComp = getComponent(entId, COMP_PHYSICS);
    if (!physComp) return;
    RotationComponent* rot = getComponent(entId, COMP_ROTATION);

    if (key_hit(KEY_LEFT | KEY_RIGHT)) {
        if (!isTaskWithFlagsInQueue(TASK_TURN_FLAG, gPlayerId, false)) {
            addTaskToQueue(gPlayerId, key_is_down(KEY_LEFT) ? TASK_TURN_LEFT : TASK_TURN_RIGHT, 16);
            }
    }
    else if (key_is_down(KEY_LEFT | KEY_RIGHT)) {
        if (!isTaskWithFlagsInQueue(TASK_TURN_FLAG, gPlayerId, true))
            addTaskToQueue(gPlayerId, key_is_down(KEY_LEFT) ? TASK_TURN_LEFT : TASK_TURN_RIGHT, 16);
    }

    if (rot) {
        SWord vecs[5] = { {0}, {0}, {0x10000}, {0}, {0} }; // vecs[0, 1, 2] are z, y, x vecs
        switch (key_is_down(KEY_ANY)) {
            case KEY_A:
            case KEY_B:
                makeRotation(&rot->mtx, 512 * key_tri_fire(), (Vector3D*)&vecs[2]);
                break;
            case KEY_L:
            case KEY_R:
                makeRotation(&rot->mtx, 512 * key_tri_shoulder(), (Vector3D*)&vecs[0]);
                break;
            default:
                break;
        }
    }

    if (key_hit(KEY_START)) { // reset
        reset();
        return;
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
