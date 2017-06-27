#include "monster.h"
#include "treasure.h"
#include "thing.h"
#include "game.h"
#include "message.h"

#include <stdio.h>
#include <stdlib.h>

#define MONSTER_MAX 32
#define TURNS_PER_SPAWN 8

#define CANDIDATE_MONSTER_COUNT 2

static Thing
make_monster (Appearance appearance, char *name, int hp, int dmg, int speed)
{
    Thing monster = make_thing (appearance, name, speed,
                                attack_bump_action,
                                chase_player_turn_action);
    monster.hp = hp;
    monster.dmg = dmg;
    return monster;
}

/* This creates a random monster, located at 0, 0. */
Thing
make_random_monster (void)
{
    static bool initialized;
    static Thing candidates[CANDIDATE_MONSTER_COUNT];

    if (!initialized)
    {
        initialized = true;

        candidates[0] =
            make_monster (GOBLIN, "Goblin", 7, 4, SPEED_DEFAULT / 2);
        candidates[1] =
            make_monster (HALFLING, "Halfling", 3, 2, SPEED_DEFAULT * 2);
    }

    int index = rand () % CANDIDATE_MONSTER_COUNT;
    return candidates[index];
}

/* Returns the number of live monsters in the game */
int
count_monsters (Game * game)
{
    int count = 0;
    for (Thing * th = NULL; next_thing (game, &th);)
    {
        if (th->turn_action == chase_player_turn_action)
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

This returns a pointer tot he monster, or NULL if it failed to
spawn one.
*/
Thing *
try_spawn_monster (Game * game)
{
    Thing *player = get_player (game);

    if (is_thing_alive (player))
    {
        int count = count_monsters (game);

        if (count < MONSTER_MAX)
        {
            if (count < (rand () % MONSTER_MAX) &&
                (rand () % TURNS_PER_SPAWN) == 0)
            {
                return place_thing (game, player->loc,
                                    make_random_monster ());
            }
        }
    }

    return NULL;
}

/* A turn action function for monsters; they chse the player. */
void
chase_player_turn_action (Game * game, Thing * actor)
{
    Thing *player = get_player (game);

    if (is_thing_alive (player))
        move_thing_towards (game, actor, player);
}

static int
roll_attack_damage (Thing * actor)
{
    /* 
       Actual damage is the average of 3 rolls from 0 to target->dmg * 2.
       Should average to actor->dmg.
     */

    int max = actor->dmg * 2;
    int dmg = 0;

    for (int i = 0; i < 3; ++i)
        dmg += rand () % (max + 1);

    return dmg / 3;
}

/* A bump-action that triggers combat */
bool
attack_bump_action (Game * game, Thing * actor, Thing * target)
{
    int dmg = roll_attack_damage (actor);

    target->hp -= dmg;

    char msg[MESSAGE_SIZE];

    if (dmg == 0)
    {
        sprintf (msg, "%s misses %s.", actor->name, target->name);
    }
    else if (target->hp > 0)
    {
        sprintf (msg, "%s hits %s for %d!", actor->name, target->name, dmg);
    }
    else
    {
        target->hp = 0;

        if (target->gold > 0)
        {
            Thing drop = make_treasure (target->gold);
            drop.loc = target->loc;
            new_thing (game, drop);
        }

        remove_thing (target);

        sprintf (msg, "%s killed %s!", actor->name, target->name);
    }

    write_game_message (game, msg);
    return false;
}
