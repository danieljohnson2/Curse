#include "player.h"
#include "monster.h"
#include "map.h"
#include "display.h"
#include "game.h"
#include "loc.h"

#include <stdlib.h>

/* This creates a player at a random place on the map. */
Thing
make_player (Map * map, TurnAction turn_action)
{
    Thing player =
        make_thing (PLAYER, "Player", SPEED_DEFAULT, attack_bump_action,
                    turn_action);

    player.loc = find_passable_place (map, make_loc (0, 0));
    player.hp = 20;
    player.dmg = 5;
    return player;
}

/*
Executes an action the user has indicated.
For 'None' this generates an error message only.
*/
void
perform_player_action (Game * game, PlayerAction action)
{
    if (action.dx != 0 || action.dy != 0)
        move_player_by (game, action.dx, action.dy);
}
