#include "map.h"
#include "thing.h"
#include "game.h"
#include "monster.h"
#include "player.h"
#include "display.h"
#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static Game game;

static void
player_turn_action (Game * game, Thing * player)
{
    PlayerAction action = None;

    do
    {
        paint (game, true);
        action = read_player_action ();
        clear_game_message (game);

        if (!is_thing_alive (player))
            return;

        perform_player_action (game, action);
    }
    while (action == None);

    try_spawn_monster (game);
}

static void
init_game (int seed)
{
    Map map = make_map (16, round_shape, make_perlin (1.0 / 8.0, 2, seed));
    Thing player = make_player (&map, player_turn_action);
    game = make_game (map, player);
}

int
main (int argc, char **argv)
{
    int seed;

    if (argc > 1)
        seed = atoi (argv[1]);
    else
    {
        srand (time (NULL));
        seed = rand ();
    }

    init_windows ();
    init_game (seed);

    perform_turns (&game);

    paint (&game, true);
    game_over ();
    end_windows ();
    return 0;
}
