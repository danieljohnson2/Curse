#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "defs.h"

void init_game (Map map, Thing player);

bool next_thing (Thing ** thing);
bool next_thing_at (Loc where, Thing ** found);

Thing *get_player (void);
Thing *new_thing (Thing thing);
Thing *place_thing (Loc origin, Thing thing);
Loc find_empty_place (Loc origin);

int get_total_gold (void);

void perform_turns (void);

#endif
