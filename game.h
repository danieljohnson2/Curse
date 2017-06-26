#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "defs.h"

Game make_game (Map map, Thing player);

Thing *get_player (Game * game);
bool find_thing_at (Game * game, Loc where, Thing ** found);

int new_thing (Game * game, Thing thing);
int place_thing (Game * game, Loc origin, Thing thing);
Loc find_empty_place (Game * game, Loc origin);

void perform_turns (Game * game);

#endif
