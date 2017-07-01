#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "loc.h"

#include <stdbool.h>

#define NAME_SIZE 64

typedef struct _Thing Thing;

typedef bool (*BumpAction) (Thing * actor, Thing * target);
typedef void (*TurnAction) (Thing * actor);

typedef enum _Appearance
{
    DEAD,
    PLAYER,
    GOBLIN,
    ORC,
    WARG,
    HALFLING,
    ELF,
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
};

Thing make_thing (Appearance appearance, char *name, int speed,
                  BumpAction bump_action, TurnAction turn_action);

#endif
