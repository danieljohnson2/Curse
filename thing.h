#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "loc.h"

#include <stdbool.h>
#include <stdio.h>

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
    WEAPON,
    SMALL_TREASURE,
    MEDIUM_TREASURE,
    LARGE_TREASURE
} Appearance;

typedef enum
{
    INERT,
    PLAYER_CONTROLLED,
    SMART_MONSTER,
    DUMB_MONSTER,
    ANIMAL,
    WEAPON_PICKUP,
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
    bool equipped;

    int remaining_wait;

    ThingBehavior behavior;
};

void define_thing_behavior (ThingBehavior behavior, BumpAction bump_action,
                            TurnAction turn_action);
BumpAction get_bump_action (Thing * thing);
TurnAction get_turn_action (Thing * thing);

Thing make_thing (Appearance appearance, char *name, int speed,
                  ThingBehavior behavior);
bool equal_things (Thing left, Thing right);

void write_thing (Thing * thing, FILE * stream);
Thing read_thing (FILE * stream);

#endif
