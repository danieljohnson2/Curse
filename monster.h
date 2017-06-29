#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "thing.h"

typedef struct _SpawnSettings
{
    int max_monsters;
    int turns_per_spawn;
} SpawnSettings;

Thing make_random_monster (void);
int count_monsters (void);
Thing *try_spawn_monster (SpawnSettings spawn);

void chase_player_turn_action (Thing * actor);
bool attack_bump_action (Thing * actor, Thing * target);

#endif
