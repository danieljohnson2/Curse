#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "defs.h"

void init_game (Game * game, Map map, Thing player);

Thing *get_player (Game * game);

bool next_thing (Game * game, Thing ** thing);
bool next_thing_at (Game * game, Loc where, Thing ** found);

Thing *get_player (Game * game);
Thing *new_thing (Game * game, Thing thing);
Thing *place_thing (Game * game, Loc origin, Thing thing);
Loc find_empty_place (Game * game, Loc origin);

int get_total_gold (Game * game);

void perform_turns (Game * game);

#endif
