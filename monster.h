#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "thing.h"

#include <stdio.h>

typedef struct SpawnSettings
{
    int max_monsters;
    int turns_per_spawn;
    int max_monster_level;
} SpawnSettings;

typedef struct MonsterPriorities
{
    double player;
    double armor;
    double weapon;
    double treasure;
    double potion;
    double other_monster;

    double focus;
} MonsterPriorities;

MonsterPriorities get_default_monster_priorities (void);
void define_monster_behavior (ThingBehavior behavior,
                              MonsterPriorities priorities);

Thing make_random_monster (int max_monster_level);
bool is_thing_monster (Thing * thing);
int count_monsters (void);
Thing *try_spawn_monster (SpawnSettings spawn);

void priority_ai_turn_action (Thing * actor);
bool attack_bump_action (Thing * actor, Thing * target);

void write_spawn_settings (SpawnSettings * settings, FILE * stream);
SpawnSettings read_spawn_settings (FILE * stream);

#endif
