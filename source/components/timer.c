#include "component.h"

void updateTimers() {
    for (int i = 0; i < gNumCompsPerType[COMP_TIMER]; i++) {
        gTimerCompsDense[i].timeRemaining--;
        if (gTimerCompsDense[i].timeRemaining == 0) {
            if (gTimerCompsDense[i].callback) {
                gTimerCompsDense[i].callback(gTimerCompsDense[i].header.entId);
                if (gTimerCompsDense[i].header.flags & TIMER_DELETE_ENT)
                    markEntToBeDeleted(gTimerCompsDense[i].header.entId);
            }
            gTimerCompsDense[i].timeRemaining = gTimerCompsDense[i].time;
        }
    }
}

void addComponentTimer(s16 entId, u16 flags, u16 length, void(*callback)) {
    TimerComponent t = { {entId, flags}, length, length, callback };
    addComponentCustom(&t, COMP_TIMER);
}

void removeComponentTimer(int entId) {
    removeComponent(entId, COMP_TIMER);
}
