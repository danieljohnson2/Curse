#include "treasure.h"
#include "game.h"

#include <stdbool.h>
#include <stdio.h>

static bool
treasure_bump_action (Game * game, Thing * actor, Thing * target)
{
    char msg[MESSAGE_MAX];
    sprintf (msg, "%s picks up %d gold!", actor->name, target->gold);
    write_game_message (game, msg);

    actor->gold += target->gold;
    remove_thing (target);
    return true;
}

/* Creates a tresure that contains the amount of gold indicated. */
Thing
make_treasure (int gold)
{
    Thing treasure =
        make_thing ('*', "Gold", 0, treasure_bump_action, null_turn_action);
    treasure.gold = gold;
    return treasure;
}
