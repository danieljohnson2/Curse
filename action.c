#include "action.h"
#include "map.h"
#include "game.h"
#include "thing.h"
#include "player.h"
#include "treasure.h"
#include "monster.h"
#include "item.h"
#include "util.h"

#include <stdlib.h>
#include <limits.h>
#include <float.h>

/*
This function sets up the behavior callbacks for each
behavior.
*/
void
init_behaviors (void)
{
    define_thing_behavior (PLAYER_CONTROLLED, attack_bump_action, NULL,
                           player_turn_action);

    define_thing_behavior (TREASURE, treasure_bump_action, NULL,
                           null_turn_action);

    define_thing_behavior (WEAPON_PICKUP, item_pickup_bump_action, NULL,
                           null_turn_action);

    define_thing_behavior (ARMOR_PICKUP, item_pickup_bump_action, NULL,
                           null_turn_action);

    define_thing_behavior (HEALING_POTION, item_pickup_bump_action,
                           healing_potion_use_action, null_turn_action);

    MonsterPriorities orc_pri = get_default_monster_priorities ();
    orc_pri.armor = 0.5;
    orc_pri.treasure = 0.25;
    orc_pri.potion = 0.0;
    orc_pri.focus = 0.75;
    define_monster_behavior (ORC_MONSTER, orc_pri);

    MonsterPriorities halfling_pri = get_default_monster_priorities ();
    halfling_pri.weapon = 0.5;
    halfling_pri.armor = 1.25;
    halfling_pri.treasure = 1.5;
    halfling_pri.potion = 1.25;
    define_monster_behavior (HALFLING_MONSTER, halfling_pri);

    MonsterPriorities elf_pri = get_default_monster_priorities ();
    elf_pri.weapon = 1.0;
    elf_pri.armor = 1.0;
    elf_pri.treasure = 0.8;
    elf_pri.potion = 1.0;
    define_monster_behavior (ELF_MONSTER, elf_pri);

    MonsterPriorities animal_pri = get_default_monster_priorities ();
    animal_pri.weapon = 0.0;
    animal_pri.armor = 0.0;
    animal_pri.treasure = 0.0;
    animal_pri.potion = 0.0;
    animal_pri.other_monster = 0.8;
    animal_pri.focus = 0.5;
    define_monster_behavior (ANIMAL, animal_pri);
}

/*
Calculates score for the path a creature will take from
'loc' to 'destination'; this is approximately the number of
turns it will take. Returns 0 if 'loc' and 'destination' are
the same DBL_MAX if the path is blocked by impassible terrain.
*/
double
measure_path (Loc loc, Loc destination)
{
    double total = 0;

    for (;;)
    {
        double step_score = try_path_step_towards (&loc, destination);

        if (step_score == 0)
            return total;

        if (step_score == DBL_MAX)
            return DBL_MAX;

        total += step_score;
    }
}

/*
Calculates a score for moving 'loc' towards 'destination';
this actually updates 'loc', and returns a score indicates how
expensive the move is. Returns if 'loc' and 'destination' already
match, and DBL_MAX if the step would be into impassible terrain. In
either of those cases, this does not update 'loc'.
*/
double
try_path_step_towards (Loc * loc, Loc destination)
{
    if (equal_locs (*loc, destination))
        return 0.0;

    int dx = destination.x - loc->x;
    int dy = destination.y - loc->y;
    Loc dests[2];

    if (abs (dx) > abs (dy))
    {
        dests[0] = offset_loc (*loc, isign (dx), 0);
        dests[1] = offset_loc (*loc, 0, isign (dy));
    }
    else
    {
        dests[0] = offset_loc (*loc, 0, isign (dy));
        dests[1] = offset_loc (*loc, isign (dx), 0);
    }

    for (int i = 0; i < 2; ++i)
    {
        if (!equal_locs (dests[i], *loc))
        {
            Terrain t = get_map_terrain (get_map (), dests[i]);
            int speed_penalty = get_terrain_speed_penalty (t);

            if (speed_penalty < INT_MAX)
            {
                *loc = dests[i];
                return (SPEED_MAX + speed_penalty) / (double) SPEED_MAX;
            }
        }
    }

    return DBL_MAX;
}

/*
Moves the 'mover' to the location indicated if possible.
If this location is impassible, this function returns false
and does nothing.
If there's something already there, this function invokes
its bump action, which may block the movement or may perform
other actions. In any of these cases, this method returns true.

True means 'something happend' here. We can't just try a
different location in such a case, lest two things happen.
*/
bool
try_move_thing_to (Thing * mover, Loc dest)
{
    Terrain t = get_map_terrain (get_map (), dest);

    int speed_penalty = get_terrain_speed_penalty (t);
    if (speed_penalty == INT_MAX)
        return false;

    for (Thing * hit = NULL;
         is_thing_alive (mover) && next_thing_at (dest, &hit);)
    {
        if (hit != mover && !get_bump_action (hit) (mover, hit))
            return true;
    }

    mover->loc = dest;
    mover->remaining_wait += speed_penalty;
    return true;
}

/* A turn action function that does nothing at all. */
void
null_turn_action (Thing * actor)
{
}

/* A bump-action that does nothing and allows movement */
bool
null_bump_action (Thing * actor, Thing * target)
{
    return true;
}
