#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include "loc.h"
#include <stdbool.h>

#define SPEED_MAX 255
#define SPEED_MIN 1

/* 16 means the MAX is 16x the default, which is 16x the minumum (1) */
#define SPEED_DEFAULT 16

typedef struct Thing Thing;

typedef bool (*BumpAction) (Thing * actor, Thing * target);
typedef void (*TurnAction) (Thing * actor);

void init_behaviors (void);

void move_thing_towards (Thing * mover, Thing * target);
bool try_move_thing_by (Thing * mover, int dx, int dy);
bool try_move_thing_to (Thing * mover, Loc dest);

void null_turn_action (Thing * actor);
bool null_bump_action (Thing * actor, Thing * target);

#endif
