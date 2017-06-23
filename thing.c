#include "thing.h"
#include "game.h"

#include <string.h>

static bool
block_action (Game * game, Thing * actor, Thing * target)
{
    write_game_message (game, "Blocked!");
    return true;
}

Thing
make_thing (char appearance, int x, int y)
{
    Thing th = { 0 };
    th.appearance = appearance;
    th.x = x;
    th.y = y;
    th.bump_action = block_action;
    return th;
}

void
move_thing_to (Game * game, Thing * mover, int x, int y)
{
    Terrain t = read_map (&game->map, x, y);

    if (!is_terrain_passable (t))
    {
        write_game_message (game, "Impassible!");
        return;
    }
    else
    {
        for (int i = 0; i < THING_COUNT; ++i)
        {
            Thing *thing = &game->things[i];

            if (thing != mover &&
                thing->appearance != '\0' && thing->x == x && thing->y == y)
            {
                if (thing->bump_action (game, mover, thing))
                    return;
            }
        }
    }

    mover->x = x;
    mover->y = y;
}
