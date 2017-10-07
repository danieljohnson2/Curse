#include "perlin.h"
#include "map.h"
#include "game.h"
#include "player.h"
#include "action.h"
#include "display.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

static char *game_over_msg[] =
    { "Game Over!", "(prese space to exit)", NULL };

/*
Exits the program with an error status and a usage message.
Used when command line arguments are not satisfactory.
*/
static void
exit_with_usage (void)
{
    char usage[] = "curse [-s <seed>] [-m round|band]";
    fprintf (stderr, "%s\n", usage);
    exit (1);
}

/* Parses the command line arguments and provide the seed and shape */
static void
read_args (int argc, char **argv, int *seed, MapShape * shape)
{
    *seed = INT_MIN;
    *shape = get_shape_from_name ("round");

    int opt;
    while ((opt = getopt (argc, argv, "s:m:h")) != -1)
    {
        switch (opt)
        {
        case 's':
            *seed = atoi (optarg);
            break;

        case 'm':
            *shape = get_shape_from_name (optarg);
            if (*shape == NULL)
                exit_with_usage ();
            break;

        case '?':
        case 'h':
            exit_with_usage ();
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

    read_args (argc, argv, &seed, &shape);

    init_windows ();
    init_behaviors ();
    Map map = make_map (16, shape, make_perlin (1.0 / 8.0, 2, seed));
    start_game (map);

    center_view (get_player ()->loc);

    arrange_windows ();
    perform_turns ();

    paint (true);

    long_message (NULL, game_over_msg);
    end_windows ();
    return 0;
}
