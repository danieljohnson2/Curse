#include "monster.h"
#include "treasure.h"
#include "game.h"
#include "action.h"
#include "message.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#define CANDIDATE_MONSTER_COUNT 5

typedef struct _MonsterData
{
    Appearance appearance;
    char *name;
    int hp;
    int dmg;
    int speed;
} MonsterData;

static MonsterData monster_data[CANDIDATE_MONSTER_COUNT] = {
    {GOBLIN, "Goblin", 7, 4, SPEED_DEFAULT / 2},
    {HALFLING, "Halfling", 3, 2, (SPEED_DEFAULT * 3) / 2},
    {ORC, "Orc", 10, 6, SPEED_DEFAULT},
    {WARG, "Warg", 8, 5, SPEED_DEFAULT * 2},
    {ELF, "Elf", 8, 9, SPEED_DEFAULT * 2}
};

static Thing
make_monster (MonsterData data)
{
    Thing monster = make_thing (data.appearance, data.name, data.speed,
                                MONSTER);
    monster.hp = data.hp;
    monster.dmg = data.dmg;
    return monster;
}

/* This creates a random monster, located at 0, 0. */
Thing
make_random_monster (int max_monster_level)
{
    static bool initialized;
    static Thing candidates[CANDIDATE_MONSTER_COUNT];

    if (!initialized)
    {
        initialized = true;

        for (int i = 0; i < CANDIDATE_MONSTER_COUNT; ++i)
            candidates[i] = make_monster (monster_data[i]);
    }

    if (max_monster_level > CANDIDATE_MONSTER_COUNT)
        max_monster_level = CANDIDATE_MONSTER_COUNT;

    int index = rand () % max_monster_level;
    return candidates[index];
}

/* Returns the number of live monsters in the game */
int
count_monsters (void)
{
    int count = 0;
    for (Thing * th = NULL; next_thing (NULL, &th);)
    {
        if (th->behavior == MONSTER)
            ++count;
    }

    return count;
}

/*
This tries to introduce a new monster in the game. It will do
this only if the player is alive and there are not already the
maximum number of monsters. Even then it will spawn only occasionally,
at random. The more monsters exist, the more rarely we spawn
new ones.

This returns a pointer to the monster, or NULL if it failed to
spawn one.
*/
Thing *
try_spawn_monster (SpawnSettings spawn)
{
    Thing *player = get_player ();

    if (is_thing_alive (player))
    {
        int count = count_monsters ();

        if (count < spawn.max_monsters)
        {
            if ((count <= (rand () % spawn.max_monsters)) &&
                ((rand () % spawn.turns_per_spawn) == 0))
            {
                return place_thing (player->loc,
                                    make_random_monster
                                    (spawn.max_monster_level));
            }
        }
    }

    return NULL;
}

/* A turn action function for monsters; they chse the player. */
void
chase_player_turn_action (Thing * actor)
{
    Thing *player = get_player ();

    if (is_thing_alive (player))
        move_thing_towards (actor, player);
}

static int
roll_attack_damage (Thing * actor, Thing ** weapon_used)
{
    /* 
       Actual damage is the average of 3 rolls from 0 to target->dmg * 2.
       Should average to actor->dmg.
     */

    int actor_dmg = actor->dmg;
    *weapon_used = NULL;

    for (Thing * th = NULL; next_thing (actor, &th);)
    {
        if (th->dmg > actor_dmg)
        {
            actor_dmg = th->dmg;
            *weapon_used = th;
        }
    }

    int max = actor_dmg * 2;
    int dmg = 0;

    for (int i = 0; i < 3; ++i)
        dmg += rand () % (max + 1);

    return dmg / 3;
}

/* A bump-action that triggers combat */
bool
attack_bump_action (Thing * actor, Thing * target)
{
    Thing *weapon_used;
    int dmg = roll_attack_damage (actor, &weapon_used);

    target->hp -= dmg;

    char msg[MESSAGE_SIZE];
    char equip_msg[MESSAGE_SIZE] = "";

    if (weapon_used != NULL)
        sprintf (equip_msg, " with %s", weapon_used->name);

    if (dmg == 0)
    {
        sprintf (msg, "%s misses %s%s.", actor->name, target->name,
                 equip_msg);
    }
    else if (target->hp > 0)
    {
        sprintf (msg, "%s hits %s for %d%s!", actor->name, target->name, dmg,
                 equip_msg);
    }
    else
    {
        target->hp = 0;

        if (target->gold > 0)
        {
            Thing drop = make_treasure (target->gold);
            drop.loc = target->loc;
            new_thing (drop);
        }

        for (Thing * item = NULL; next_thing (target, &item);)
        {
            Thing *dropped = new_thing (*item);
            if (dropped != NULL)
                dropped->loc = target->loc;
            remove_thing (item);
        }

        remove_thing (target);

        sprintf (msg, "%s killed %s%s!", actor->name, target->name,
                 equip_msg);
    }

    write_message (msg);
    return false;
}

void
write_spawn_settings (SpawnSettings * settings, FILE * stream)
{
    named_writef (stream, "max_monsters", "%d", settings->max_monsters);
    named_writef (stream, "turns_per_spawn", "%d", settings->turns_per_spawn);
    named_writef (stream, "max_monster_level", "%d",
                  settings->max_monster_level);
}

SpawnSettings
read_spawn_settings (FILE * stream)
{
    SpawnSettings spawn = { 0 };

    readf (stream, "max_monsters", "%d", &spawn.max_monsters);
    readf (stream, "turns_per_spawn", "%d", &spawn.turns_per_spawn);
    readf (stream, "max_monster_level", "%d", &spawn.max_monster_level);

    return spawn;
}
