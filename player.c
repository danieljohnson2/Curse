#include "player.h"
#include "monster.h"
#include "map.h"

/* This creates a player at a random place on the map. */
Thing
make_player (Map * map, TurnAction turn_action,
             TurnAction skipped_turn_action)
{
    Thing player =
        make_thing ('@', "Player", SPEED_DEFAULT, attack_bump_action,
                    null_turn_action);

    find_passable_place (map, 0, 0, &player.x, &player.y);
    player.turn_action = turn_action;
    player.skipped_turn_action = skipped_turn_action;
    return player;
}
