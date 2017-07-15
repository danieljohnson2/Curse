#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "loc.h"

#include <stdbool.h>

#define NAME_SIZE 64

typedef struct Thing Thing;

typedef bool (*BumpAction) (Thing * actor, Thing * target);
typedef void (*TurnAction) (Thing * actor);

typedef enum
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

typedef enum
{
    INERT,
    PLAYER_CONTROLLED,
    MONSTER,
    TREASURE
} ThingBehavior;

struct Thing
{
    Appearance appearance;
    char name[NAME_SIZE];

    Loc loc;
    int gold;
    int hp;
    int dmg;
    int speed;

    int remaining_wait;

    ThingBehavior behavior;
};

void define_thing_behavior (ThingBehavior behavior, BumpAction bump_action,
                            TurnAction turn_action);
BumpAction get_bump_action (Thing * thing);
TurnAction get_turn_action (Thing * thing);

Thing make_thing (Appearance appearance, char *name, int speed,
                  ThingBehavior behavior);

#endif
