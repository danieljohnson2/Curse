#include "treasure.h"
#include "game.h"
#include "action.h"
#include "message.h"

#include <stdio.h>
#include <stdlib.h>

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

    Thing treasure = make_thing (ap, "Gold", 0, TREASURE);
    treasure.gold = gold;
    return treasure;
}

/*
This function is the bump action to allow you to pick up
treasure.
*/
bool
treasure_bump_action (Thing * actor, Thing * target)
{
    char msg[MESSAGE_SIZE];
    sprintf (msg, "%s picks up %d gold!", actor->name, target->gold);
    write_message (msg);

    actor->gold += target->gold;
    remove_thing (target);
    return true;
}
