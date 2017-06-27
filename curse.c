#include "perlin.h"
#include "map.h"
#include "game.h"
#include "player.h"
#include "thing.h"
#include "message.h"
#include "display.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

static Game game;

static void
player_turn_action (Game * game, Thing * player)
{
    paint (game, true);
    PlayerAction action = read_player_action ();
    clear_game_message (game);

    if (is_thing_alive (player))
        perform_player_action (game, action);
}

static void
init_game (int seed, MapShape shape)
{
    Map map = make_map (16, shape, make_perlin (1.0 / 8.0, 2, seed));
    Thing player = make_player (&map, player_turn_action);
    game = make_game (map, player);
}

/*
Exits the program with an error status and a usage message.
Used when command line arguments are not satisfactory.
*/
static void
exit_with_usage()
{
	char usage[] = "curse [-s <seed>] [-m round|band]";
	fprintf(stderr, "%s\n", usage);
	exit(1);
}

/* Parses the command line arguments and provide the seed and shape */
static void
read_args(int argc, char **argv, int *seed, MapShape *shape)
{
	*seed = INT_MIN;
	*shape = band_shape;
	
	int opt;
	while ((opt = getopt(argc, argv, "s:m:"))!=-1)
	{
		switch(opt)
		{
			case 's':
			    *seed = atoi (optarg);
			    break;
		    
		    case 'm':
		    	*shape = get_shape_name(optarg);
		    	if (*shape == NULL)
		    		exit_with_usage();
		    	break;
		    	
			case '?':
				exit_with_usage();
				break;
		}
	}
	
	if (*seed == INT_MIN)
    {
        srand (time (NULL));
        *seed = rand ();
    }
}

int
main (int argc, char **argv)
{
    int seed;
    MapShape shape;
    
	read_args(argc, argv, &seed, &shape);
	
    init_windows ();
    init_game (seed, shape);

    perform_turns (&game);

    paint (&game, true);
    game_over ();
    end_windows ();
    return 0;
}
