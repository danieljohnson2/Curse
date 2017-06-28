#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stddef.h>
#include <stdbool.h>

/*
This file contains type definitions shared by all modules;
this lets us return structs by value since everyone has the
definitions. Changing this file will cause everything
to rebuild, but changing other header's won't.
*/

#define PERLIN_HASH_SIZE 4096
#define THING_COUNT 32

#define MESSAGE_SIZE 512
#define NAME_SIZE 64

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

typedef bool (*BumpAction) (Thing * actor, Thing * target);
typedef void (*TurnAction) (Thing * actor);

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
    Thing things[THING_COUNT];
} Game;

extern Game game;

typedef struct _PlayerAction
{
    int dx, dy;
} PlayerAction;

#endif
