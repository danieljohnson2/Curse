#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "thing.h"

typedef struct _Game Game;

#define MONSTER_MAX 32
#define TURNS_PER_SPAWN 8

Thing make_monster ();
int count_monsters (Game * game);
int try_spawn_monster (Game * game);

void chase_player_turn_action (Game * game, Thing * actor);
bool attack_bump_action (Game * game, Thing * actor, Thing * target);

#endif
