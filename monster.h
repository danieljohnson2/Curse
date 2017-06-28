#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "thing.h"

Thing make_random_monster (void);
int count_monsters (void);
Thing *try_spawn_monster (void);

void chase_player_turn_action (Thing * actor);
bool attack_bump_action (Thing * actor, Thing * target);

#endif
