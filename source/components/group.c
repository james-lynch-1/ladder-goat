#include "component.h"

GroupComponent* addComponentGroup(s16 entId, u16 flags, s16* memberIdsArr, int numMembers) {
    GroupComponent g = { {entId, flags}, {}, numMembers };
    memset16(&g.memberIds, -1, MAX_MEMBERS_PER_GROUP);
    if (memberIdsArr && (numMembers > 0))
        for (int i = 0; i < numMembers; i++) {
            MemberComponent* mComp = getComponent(memberIdsArr[i], COMP_MEMBER);
            if (!mComp) addComponentMember(memberIdsArr[i], 0, entId);
            g.memberIds[i] = memberIdsArr[i];
        }
    return (GroupComponent*)addComponentCustom(&g, COMP_GROUP);
}

void removeComponentGroup(int entId) {
    removeComponent(entId, COMP_GROUP);
}
