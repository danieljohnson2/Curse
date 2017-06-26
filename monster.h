#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "defs.h"

Thing make_random_monster (void);
int count_monsters (Game * game);
int try_spawn_monster (Game * game);

void chase_player_turn_action (Game * game, Thing * actor);
bool attack_bump_action (Game * game, Thing * actor, Thing * target);

#endif
