#include "component.h"

GroupComponent* addComponentGroup(s16 entId, u16 flags, s16* memberIds, void (*fn)(struct MemberComponent_*, struct GroupComponent_*), int numMembers, int entKind) {
    GroupComponent g = { {entId, 0}, {-1, -1, -1, -1}, NULL, 0, numMembers };
    if (memberIds && (numMembers > 0))
        for (int i = 0; i < numMembers; i++) {
            MemberComponent* mComp = getComponent(memberIds[i], COMP_MEMBER);
            if (!mComp) addComponentMember(memberIds[i], entKind, entId);
            g.memberIds[i] = memberIds[i];
        }
    g.fn = fn;
    return (GroupComponent*)addComponentCustom(&g, COMP_GROUP);
}

void removeComponentGroup(int entId) {
    removeComponent(entId, COMP_GROUP);
}
