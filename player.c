#include "player.h"
#include "monster.h"
#include "map.h"
#include "display.h"
#include "game.h"

#include <stdlib.h>

/* This creates a player at a random place on the map. */
Thing
make_player (Map * map, TurnAction turn_action,
             TurnAction skipped_turn_action)
{
    Thing player =
        make_thing ('@', "Player", SPEED_DEFAULT, attack_bump_action,
                    null_turn_action);

    player.loc = find_passable_place (map, make_loc (0, 0));
    player.turn_action = turn_action;
    player.skipped_turn_action = skipped_turn_action;
    return player;
}

/*
Executes an action the user has indicated.
For 'None' this generates an error message only.
*/
void
perform_player_action (Game * game, PlayerAction action)
{
    switch (action)
    {
    case Quit:
        end_windows ();
        exit (0);
        break;
    case Up:
        move_player_by (game, 0, -1);
        break;
    case Down:
        move_player_by (game, 0, +1);
        break;
    case Left:
        move_player_by (game, -1, 0);
        break;
    case Right:
        move_player_by (game, +1, 0);
        break;
    case Pass:
        break;
    default:
        write_game_message (game, "Invalid key.");
        break;
    }
}
