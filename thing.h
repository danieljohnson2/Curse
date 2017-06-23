#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <stdbool.h>

#define THING_COUNT 16
#define PLAYER_INDEX 0
#define NAME_MAX 64

typedef struct _Game Game;
typedef struct _Thing Thing;
typedef struct _Map Map;

typedef bool (*BumpAction) (Game * game, Thing * actor, Thing * target);
typedef void (*TurnAction) (Game * game, Thing * actor);

struct _Thing
{
    char appearance;
    char name[NAME_MAX + 1];
    int x, y;
    BumpAction bump_action;
    TurnAction turn_action;
};

Thing make_thing (char appearance, char *name, int x, int y,
                  BumpAction bump_action, TurnAction turn_action);

void remove_thing (Thing * thing);
bool is_thing_alive (Thing * thing);

void move_thing_towards (Game * game, Thing * mover, Thing * target);
bool try_move_thing_by (Game * game, Thing * mover, int dx, int dy);
void move_player_by (Game * game, int dx, int dy);
bool try_move_thing_to (Game * game, Thing * mover, int x, int y);

void null_turn_action (Game * game, Thing * actor);
void chase_player_turn_action (Game * game, Thing * actor);

bool null_bump_action (Game * game, Thing * actor, Thing * target);
bool attack_bump_action (Game * game, Thing * actor, Thing * target);

#endif
