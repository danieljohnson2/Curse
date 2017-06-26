#include "thing.h"
#include "monster.h"
#include "player.h"
#include "map.h"
#include "message.h"
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

/* Returns the player (who might be dead!) */
Thing *
get_player (Game * game)
{
    return &game->things[game->player_index];
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

    try_spawn_monster (game);
}

/*
Moves the player by an offset; this may trigger bump actions,
and writes a message (rather than retrying) if the player moves
into impassible terrain.
*/
void
move_player_by (Game * game, int dx, int dy)
{
    Thing *player = get_player (game);

    if (!try_move_thing_by (game, player, dx, dy))
        write_game_message (game, "Impassible!");
}
