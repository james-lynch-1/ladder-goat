#ifndef CONSTANTS
#define CONSTANTS

#define ANGLE_MIN           0
#define ANGLE_MAX           65535
#define PLAYER_MOVE_SPEED   0x10000 // units/frame (16.16)
#define MAX_ENTS            125

#define MAX_TASKS           128

#define MAX_EVENT_LISTENERS_PER_TYPE    32

#define MAX_OBJ_COMPONENTS              78
#define MAX_OBJ_AFF_COMPONENTS          32
#define MAX_INPUT_COMPONENTS            8
#define MAX_PHYSICS_COMPONENTS          32
#define MAX_ROTATION_COMPONENTS         8
#define MAX_TIMER_COMPONENTS            128
#define MAX_COUNTER_COMPONENTS          128
#define MAX_MEMBER_COMPONENTS           128
#define MAX_GROUP_COMPONENTS            8
#define MAX_TASK_QUEUE_COMPONENTS       32
#define MAX_CELL_COMPONENTS             82

#define MAX_GROUPS_PER_MEMBER           4
#define MAX_MEMBERS_PER_GROUP           24

#define MAX_PHYS_ARCHETYPES_STATIC      32

#define MAX_ALLOC_SPRITES               128

// enemy-related constants
#define WEAK_TURN_FREQ      20

#define PLAYER_HEIGHT       32

#define NUM_SPRITE_CELLS    26
#define ATTR0_GFX_MASK      0x0C00

#define MAP_WIDTH_X         19
#define MAP_WIDTH_Z         19
#define NUM_ROWS            21
#define NUM_COLS            16

// Metatile width, in pixels
#define MT_WIDTH            16
#define SBB_WIDTH_MT        16
#define MT_PER_SBB          256
#define TILE_WIDTH          8
#define SBB_WIDTH_T         32
#define SBB_HEIGHT_T        32
#define TILES_PER_SBB       1024

// BG defines
#define BG_MAP              0
#define BG_UI               1
#define BG_TEXT             2

// SBB defines
#define UI_SBB              26
#define TEXT_SBB            27
#define MAP_SBB             31

// CBB defines
#define CBB_MAP             0
#define CBB_UI              1
#define CBB_TEXT            2

// BG palette defines
#define TEXT_PAL            0
#define UI_PAL              1
#define MAP_PAL             2

// BG priority defines
#define PRIO_MAP            3
#define PRIO_UI             2
#define PRIO_TEXT           1

// global flags (gFlags)

// #define GFLAG_EXAMPLE    0x00000001

// ent flags (in gEntFlags)
#define ENT_TO_DELETE           0b10000000
#define ENT_IN_USE              0b00000001

#endif
