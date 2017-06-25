#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "loc.h"

#include <stdbool.h>

#define THING_COUNT 32
#define PLAYER_INDEX 0

#define NAME_SIZE 64

#define SPEED_MAX 255
#define SPEED_MIN 1

/* 16 means the MAX is 16x the default, which is 16x the minumum (1) */
#define SPEED_DEFAULT 16

typedef struct _Game Game;
typedef struct _Thing Thing;
typedef struct _Map Map;

typedef bool (*BumpAction) (Game * game, Thing * actor, Thing * target);
typedef void (*TurnAction) (Game * game, Thing * actor);

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
    TurnAction skipped_turn_action;
};

Thing make_thing (Appearance appearance, char *name, int speed,
                  BumpAction bump_action, TurnAction turn_action);

void remove_thing (Thing * thing);
bool is_thing_alive (Thing * thing);

void move_thing_towards (Game * game, Thing * mover, Thing * target);
bool try_move_thing_by (Game * game, Thing * mover, int dx, int dy);
void move_player_by (Game * game, int dx, int dy);
bool try_move_thing_to (Game * game, Thing * mover, Loc dest);

void null_turn_action (Game * game, Thing * actor);

bool null_bump_action (Game * game, Thing * actor, Thing * target);

#endif
