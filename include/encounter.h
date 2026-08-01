#ifndef ENCOUNTER
#define ENCOUNTER

#include "global.h"
#include "graphics.h"
#include "mem.h"
#include "component.h"
#include "eventListener.h"
#include "player.h"
#include "ui.h"

extern PaletteSet defaultPaletteSet;

extern Encounter firstEncounter;

void loadEncounter(Encounter* encounter);

#endif
