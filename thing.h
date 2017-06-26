#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "defs.h"

#define SPEED_MAX 255
#define SPEED_MIN 1

/* 16 means the MAX is 16x the default, which is 16x the minumum (1) */
#define SPEED_DEFAULT 16

Thing make_thing (Appearance appearance, char *name, int speed,
                  BumpAction bump_action, TurnAction turn_action);

void remove_thing (Thing * thing);
bool is_thing_alive (Thing * thing);

void move_thing_towards (Game * game, Thing * mover, Thing * target);
bool try_move_thing_by (Game * game, Thing * mover, int dx, int dy);
bool try_move_thing_to (Game * game, Thing * mover, Loc dest);

void null_turn_action (Game * game, Thing * actor);

bool null_bump_action (Game * game, Thing * actor, Thing * target);

#endif
