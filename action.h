#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include "loc.h"
#include "thing.h"
#include <stdbool.h>

#define SPEED_MAX 255
#define SPEED_MIN 1

/* 16 means the MAX is 16x the default, which is 16x the minumum (1) */
#define SPEED_DEFAULT 16

void init_behaviors (void);

double measure_path (Loc loc, Loc destination);
double try_path_step_towards (Loc * loc, Loc destination);

bool try_move_thing_to (Thing * mover, Loc dest);

void null_turn_action (Thing * actor);
bool null_bump_action (Thing * actor, Thing * target);

#endif
