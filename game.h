#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "map.h"
#include "thing.h"

typedef struct _Thing Thing;

void init_game (Map map, Thing player);
void next_level (void);

bool next_thing (Thing ** thing);
bool next_thing_at (Loc where, Thing ** found);

Map *get_map (void);
Thing *get_player (void);
Thing *new_thing (Thing thing);
Thing *place_thing (Loc origin, Thing thing);
Loc find_empty_place (Loc origin);

void remove_thing (Thing * thing);
bool is_thing_alive (Thing * thing);

int get_total_gold (void);

void perform_turns (void);

#endif
