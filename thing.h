#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <stdbool.h>

#define THING_COUNT 16
#define PLAYER_INDEX 0
#define NAME_MAX 64

typedef struct _Game Game;
typedef struct _Thing Thing;

typedef bool (*BumpAction) (Game * game, Thing * actor, Thing * target);

struct _Thing
{
    char appearance;
    char name[NAME_MAX + 1];
    int x, y;
    BumpAction bump_action;
};

Thing make_thing (char appearance, char *name, int x, int y,
                  BumpAction bump_action);

void remove_thing (Thing * thing);
void move_thing_to (Game * game, Thing * mover, int x, int y);

bool null_bump_action (Game * game, Thing * actor, Thing * target);
bool attack_bump_action (Game * game, Thing * actor, Thing * target);

#endif
