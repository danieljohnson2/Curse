#include "thing.h"
#include "game.h"

#include <string.h>
#include <stdio.h>

static int
iabs (int value)
{
    if (value >= 0)
        return value;
    else
        return -value;
}

static int
isign (int value)
{
    if (value > 0)
        return 1;
    else if (value < 0)
        return -1;
    else
        return 0;
}

Thing
make_thing (char appearance, char *name, int x, int y, BumpAction bump_action,
            TurnAction turn_action)
{
    Thing th = { 0 };
    th.appearance = appearance;
    strcpy (th.name, name);
    th.x = x;
    th.y = y;
    th.bump_action = bump_action;
    th.turn_action = turn_action;
    return th;
}

void
remove_thing (Thing * thing)
{
    thing->appearance = '\0';
}

void
move_thing_towards (Game * game, Thing * mover, Thing * target)
{
    int dx = target->x - mover->x;
    int dy = target->y - mover->y;

    if (iabs (dx) > iabs (dy))
        move_thing_by (game, mover, isign (dx), 0);
    else
        move_thing_by (game, mover, 0, isign (dy));
}

void
move_thing_by (Game * game, Thing * mover, int dx, int dy)
{
    move_thing_to (game, mover, mover->x + dx, mover->y + dy);
}

void
move_thing_to (Game * game, Thing * mover, int x, int y)
{
    Terrain t = read_map (&game->map, x, y);

    if (!is_terrain_passable (t))
    {
        if (mover == &game->things[PLAYER_INDEX])
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

void
null_turn_action (Game * game, Thing * actor)
{
}

void
chase_player_turn_action (Game * game, Thing * actor)
{
    Thing *player = &game->things[PLAYER_INDEX];

    if (player->appearance != '\0')
        move_thing_towards (game, actor, player);
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
    sprintf (msg, "%s killed %s!", actor->name, target->name);
    write_game_message (game, msg);
    return true;
}
