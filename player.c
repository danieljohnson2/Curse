#include "player.h"
#include "action.h"
#include "monster.h"
#include "game.h"
#include "map.h"
#include "message.h"
#include "display.h"

#include <stdlib.h>

static char *level_clear_msg[] =
    { "Level Cleared!", "(press space for next level)", NULL };

static void
player_turn_action (Thing * player)
{
    paint (true);

    if (player->gold >= get_total_gold ())
    {
        long_message (level_clear_msg);
        player->gold = 0;
        next_level ();
        paint (true);
    }

    PlayerAction action = read_player_action ();

    if (is_thing_alive (player))
        perform_player_action (action);
}

/* This creates a player at a random place on the map. */
Thing
make_player (Map * map)
{
    define_thing_behavior (PLAYER_CONTROLLED, attack_bump_action,
                           player_turn_action);

    Thing player =
        make_thing (PLAYER, "Player", SPEED_DEFAULT, PLAYER_CONTROLLED);

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
perform_player_action (PlayerAction action)
{
    if (action.dx != 0 || action.dy != 0)
        move_player_by (action.dx, action.dy);
}

/*
Moves the player by an offset; this may trigger bump actions,
and writes a message (rather than retrying) if the player moves
into impassible terrain.
*/
void
move_player_by (int dx, int dy)
{
    Thing *player = get_player ();

    if (!try_move_thing_by (player, dx, dy))
        write_message ("Impassible!");
}
