#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stddef.h>
#include <stdbool.h>

#define PERLIN_HASH_SIZE 4096
#define THING_COUNT 32
#define PLAYER_INDEX 0

#define MESSAGE_SIZE 512
#define NAME_SIZE 64

#define SPEED_MAX 255
#define SPEED_MIN 1

/* 16 means the MAX is 16x the default, which is 16x the minumum (1) */
#define SPEED_DEFAULT 16

typedef struct _Loc
{
    int x, y;
} Loc;

typedef struct _Perlin
{
    double freq;
    int depth;
    double origin_offset;

    unsigned char hash[PERLIN_HASH_SIZE];
} Perlin;

typedef enum _Terrain
{
    DEEP_SEA,
    WATER,
    BEACH,
    GRASS,
    WOODS,
    HILLS,
    MOUNTAINS
} Terrain;

typedef struct _Map Map;
typedef double (*MapShape) (Map * map, Loc where);

struct _Map
{
    double soft_size;
    MapShape shape;
    Perlin perlin;
};

typedef enum _Appearance
{
    DEAD,
    PLAYER,
    GOBLIN,
    HALFLING,
    SMALL_TREASURE,
    MEDIUM_TREASURE,
    LARGE_TREASURE
} Appearance;

typedef struct _Thing Thing;
typedef struct _Game Game;

typedef bool (*BumpAction) (Game * game, Thing * actor, Thing * target);
typedef void (*TurnAction) (Game * game, Thing * actor);

struct _Thing
{
    Appearance appearance;
    char name[NAME_SIZE];

    Loc loc;
    int gold;
    int hp;
    int dmg;
    int speed;

    int remaining_wait;

    BumpAction bump_action;
    TurnAction turn_action;
};

typedef struct _Game
{
    Map map;
    Loc view_center;

    char message[MESSAGE_SIZE];
    Thing things[THING_COUNT];
} Game;

typedef struct _PlayerAction
{
    int dx, dy;
} PlayerAction;

#endif
