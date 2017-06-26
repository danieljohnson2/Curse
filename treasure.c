#include "treasure.h"
#include "thing.h"
#include "message.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool
treasure_bump_action (Game * game, Thing * actor, Thing * target)
{
    char msg[MESSAGE_SIZE];
    sprintf (msg, "%s picks up %d gold!", actor->name, target->gold);
    write_game_message (game, msg);

    actor->gold += target->gold;
    remove_thing (target);
    return true;
}

/* Creates a treasure of a random size */
Thing
make_random_treasure (void)
{
    int gold = (rand () % 41) + 3;
    return make_treasure (gold);
}

/* Creates a treasure that contains the amount of gold indicated. */
Thing
make_treasure (int gold)
{
    Appearance ap = MEDIUM_TREASURE;

    if (gold <= 5)
        ap = SMALL_TREASURE;
    else if (gold >= 30)
        ap = LARGE_TREASURE;

    Thing treasure =
        make_thing (ap, "Gold", 0, treasure_bump_action, null_turn_action);
    treasure.gold = gold;
    return treasure;
}
