#include "monster.h"
#include "treasure.h"
#include "item.h"
#include "game.h"
#include "action.h"
#include "message.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CANDIDATE_MONSTER_COUNT 5

typedef struct _MonsterData
{
    Appearance appearance;
    char *name;
    int hp;
    int dmg;
    int speed;
    ThingBehavior behavior;
} MonsterData;

static MonsterData monster_data[CANDIDATE_MONSTER_COUNT] = {
    {GOBLIN, "Goblin", 7, 4, SPEED_DEFAULT / 2, ORC_MONSTER},
    {HALFLING, "Halfling", 3, 2, (SPEED_DEFAULT * 3) / 2, HALFLING_MONSTER},
    {ORC, "Orc", 10, 6, SPEED_DEFAULT, ORC_MONSTER},
    {WARG, "Warg", 8, 5, SPEED_DEFAULT * 2, ANIMAL},
    {ELF, "Elf", 8, 9, SPEED_DEFAULT * 2, ELF_MONSTER}
};

static Thing
make_monster (MonsterData data)
{
    Thing monster = make_thing (data.appearance, data.name, data.speed,
                                data.behavior);
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

/* Checks to see if a thing is a monster, which is determined by its
behavior */
bool
is_thing_monster (Thing * thing)
{
    return thing->behavior >= ORC_MONSTER && thing->behavior <= ANIMAL;
}

/* Returns the number of live monsters in the game */
int
count_monsters (void)
{
    int count = 0;
    for (Thing * th = NULL; next_thing (NULL, &th);)
    {
        if (is_thing_monster (th))
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

static bool
is_thing_targetable (Thing * actor, Thing * candidate)
{
    if (candidate == NULL)
        return false;

    ThingBehavior b = candidate->behavior;

    if (b == PLAYER_CONTROLLED)
        return true;

    if (actor->behavior != ANIMAL)
    {
        if (b == WEAPON_PICKUP || b == ARMOR_PICKUP || b == TREASURE)
            return true;
    }

    return false;
}

/* This works out a score to indicate how interesting
a candidate is to an actor; the actor picks the candidate
with the _lowest_ score. */
static double
thing_interest_score (Thing * actor, Thing * candidate)
{
    int dx = candidate->loc.x - actor->loc.x;
    int dy = candidate->loc.y - actor->loc.y;
    double score = sqrt ((dx * dx) + (dy * dy));

    if (candidate->behavior == PLAYER_CONTROLLED)
        return score / 2.0;
    else
        return score;
}

/* A turn action function for monsters; they chse the player. */
void
chase_player_turn_action (Thing * actor)
{
    Thing *target = actor->target;

    if (target == NULL || !is_thing_targetable (actor, target))
    {
        target = NULL;
        double target_score = 0;

        for (Thing * th = NULL; next_thing (NULL, &th);)
        {
            if (is_thing_targetable (actor, th))
            {
                double score = thing_interest_score (actor, th);

                if (target == NULL || target_score > score)
                {
                    target_score = score;
                    target = th;
                }
            }
        }
    }

    if (is_thing_targetable (actor, target))
        if (!try_move_thing_towards (actor, target))
            actor->target = NULL;       // try a different target next time!
}

static int
roll_attack_damage (Thing * actor, Thing * weapon, Thing * armor)
{
    /* 
       Actual damage is the average of 3 rolls from 0 to target->dmg * 2.
       Should average to actor->dmg.
     */

    int actor_dmg = weapon == NULL ? actor->dmg : weapon->dmg;
    int max = actor_dmg * 2;
    int dmg = 0;

    for (int i = 0; i < 3; ++i)
        dmg += rand () % (max + 1);

    dmg /= 3;

    if (armor != NULL)
    {
        int limit = rand () & (armor->dmg + 1);
        if (dmg < limit)
            dmg = 0;
    }

    return dmg;
}

/* A bump-action that triggers combat */
bool
attack_bump_action (Thing * actor, Thing * target)
{
    Thing *weapon = get_equipped_item (actor, WEAPON_PICKUP);
    Thing *armor = get_equipped_item (target, ARMOR_PICKUP);
    int dmg = roll_attack_damage (actor, weapon, armor);

    target->hp -= dmg;

    char msg[MESSAGE_SIZE];
    char equip_msg[MESSAGE_SIZE] = "";

    if (weapon != NULL)
        sprintf (equip_msg, " with %s", weapon->name);

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
            {
                dropped->loc = target->loc;
                dropped->equipped = false;
            }
            remove_thing (item);
        }

        sprintf (msg, "%s killed %s%s!", actor->name, target->name,
                 equip_msg);

        remove_thing (target);
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
