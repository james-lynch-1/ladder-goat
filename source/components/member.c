#include "component.h"

MemberComponent* addComponentMember(s16 entId, u16 flags, int groupId) {
    MemberComponent m = { {entId, flags}, {groupId, -1, -1, -1} };
    MemberComponent* mAddr = (MemberComponent*)addComponentCustom(&m, COMP_MEMBER);
    GroupComponent* group = getComponent(groupId, COMP_GROUP);
    if (group) group->memberIds[group->numMembers++] = entId;
    return mAddr;
}

void removeComponentMember(int entId) {
    MemberComponent* m = getComponent(entId, COMP_MEMBER);
    GroupComponent* g;
    for (int i = 0; i < MAX_GROUPS_PER_MEMBER; i++) {
        g = getComponent(m->groupIds[i], COMP_GROUP);
        if (!g) break;
        for (int i = 0; i < g->numMembers; i++) {
            if (g->memberIds[i] == entId) {
                g->memberIds[i] = g->memberIds[--(g->numMembers)];
                break;
            }
        }
    }
    removeComponent(entId, COMP_MEMBER);
}

void removeMemberCompFromGroup(int entIdMem, int entIdGroup) {
    MemberComponent* m = getComponent(entIdMem, COMP_MEMBER);
    GroupComponent* g = getComponent(entIdGroup, COMP_GROUP);
    if (!m || !g) return;
    for (int i = 0; i < MAX_GROUPS_PER_MEMBER; i++) {
        if (m->groupIds[i] == g->header.entId) {
            m->groupIds[i] = -1;
            break;
        }
    }
    for (int i = 0; i < MAX_MEMBERS_PER_GROUP; i++) {
        if (g->memberIds[i] == m->header.entId) {
            g->memberIds[i] = --(g->numMembers);
            break;
        }
    }
}

void doGroupCallbacks(int entId) {
    MemberComponent* m = getComponent(entId, COMP_MEMBER);
    if (!m) return;
    for (int i = 0; i < MAX_GROUPS_PER_MEMBER; i++) {
        GroupComponent* g = getComponent(m->groupIds[i], COMP_GROUP);
        if (!g) break;
        if (g->fn) g->fn(m, g);
    }
}
