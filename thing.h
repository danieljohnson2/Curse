#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <stdbool.h>

#define THING_COUNT 16
#define PLAYER_INDEX 0

typedef struct _Game Game;
typedef struct _Thing Thing;

typedef bool (*BumpAction) (Game * game, Thing * actor, Thing * target);

struct _Thing
{
    char appearance;
    int x, y;
    BumpAction bump_action;
};

Thing make_thing (char appearance, int x, int y);
void move_thing_to (Game * game, Thing * mover, int x, int y);

#endif
