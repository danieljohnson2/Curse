#include "thing.h"
#include "game.h"

#include <string.h>

Thing
make_thing (char appearance, char *name, int x, int y, BumpAction bump_action)
{
    Thing th = { 0 };
    th.appearance = appearance;
    strcpy (th.name, name);
    th.x = x;
    th.y = y;
    th.bump_action = bump_action;
    return th;
}

void
remove_thing (Thing * thing)
{
    thing->appearance = '\0';
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

bool
null_bump_action (Game * game, Thing * actor, Thing * target)
{
    return false;
}

bool
attack_bump_action (Game * game, Thing * actor, Thing * target)
{
    remove_thing (target);

    char msg[MESSAGE_MAX];
    sprintf (msg, "%s killed %s", actor->name, target->name);
    write_game_message (game, msg);
    return true;
}
