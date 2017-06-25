#include "monster.h"
#include "treasure.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>

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

    for (int i = 0; i < THING_COUNT; ++i)
    {
        Thing *th = &game->things[i];
        if (is_thing_alive (th)
            && th->turn_action == chase_player_turn_action)
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

This returns the index of the new moster, or -1 if it failed to
spawn one.
*/
int
try_spawn_monster (Game * game)
{
    Thing *player = &game->things[PLAYER_INDEX];

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

    return -1;
}

/* A turn action function for monsters; they chse the player. */
void
chase_player_turn_action (Game * game, Thing * actor)
{
    Thing *player = &game->things[PLAYER_INDEX];

    if (is_thing_alive (player))
        move_thing_towards (game, actor, player);
}

/* A bump-action that triggers combat */
bool
attack_bump_action (Game * game, Thing * actor, Thing * target)
{
    target->hp -= actor->dmg;

    if (target->hp <= 0)
    {
        target->hp = 0;

        if (target->gold > 0)
        {
            Thing drop = make_treasure (target->gold);
            drop.loc = target->loc;
            new_thing (game, drop);
        }

        remove_thing (target);

        char msg[MESSAGE_MAX];
        sprintf (msg, "%s killed %s!", actor->name, target->name);
        write_game_message (game, msg);
        return false;
    }
    else
    {
        char msg[MESSAGE_MAX];
        sprintf (msg, "%s hits %s!", actor->name, target->name);
        write_game_message (game, msg);
        return false;
    }
}
