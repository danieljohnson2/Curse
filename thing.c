#include "thing.h"
#include "util.h"
#include "action.h"

#include <memory.h>

#define BEHAVIOR_MAX 16

static struct
{
    BumpAction bump_action;
    TurnAction turn_action;
} behaviorDefs[BEHAVIOR_MAX];

/*
This function sets the function pointers that define the behavior of
a particular type of thing, identifed by 'behavior'.
*/
void
define_thing_behavior (ThingBehavior behavior, BumpAction bump_action,
                       TurnAction turn_action)
{
    behaviorDefs[behavior].bump_action = bump_action;
    behaviorDefs[behavior].turn_action = turn_action;
}

/*
This function provides the bump action function for a thing;
this does not return NULL, but will return a do-nothing function
instead.
*/
BumpAction
get_bump_action (Thing * thing)
{
    BumpAction action = behaviorDefs[thing->behavior].bump_action;

    if (action != NULL)
        return action;
    else
        return null_bump_action;
}

/*
This function provides the turn action function for a thing;
this does not return NULL, but will return a do-nothing function
instead.
*/
TurnAction
get_turn_action (Thing * thing)
{
    TurnAction action = behaviorDefs[thing->behavior].turn_action;

    if (action != NULL)
        return action;
    else
        return null_turn_action;
}

/*
Create a thing in a single call.
*/
Thing
make_thing (Appearance appearance, char *name, int speed,
            ThingBehavior behavior)
{
    Thing th = { 0 };
    th.appearance = appearance;
    strtcpy (th.name, name, NAME_SIZE);
    th.speed = speed;
    th.remaining_wait = 0;
    th.behavior = behavior;
    return th;
}

/*
Compares two things to see if they are the same. This
ignores the 'remaining wait' and 'loc'.
*/
bool
equal_things (Thing left, Thing right)
{
    return
        left.appearance == right.appearance &&
        strcmp (left.name, right.name) == 0 &&
        left.gold == right.gold &&
        left.hp == right.hp &&
        left.dmg == right.dmg &&
        left.speed == right.speed && left.behavior == right.behavior;
}

/*
Writes the data for one thing to a stream.
*/
void
write_thing (Thing * thing, FILE * stream)
{
    named_writef (stream, "appearance", "%d", (int) thing->appearance);

    if (thing->appearance != DEAD)
    {
        named_writef (stream, "name", "%s", thing->name);
        named_writef (stream, "x", "%d", thing->loc.x);
        named_writef (stream, "y", "%d", thing->loc.y);

        named_writef (stream, "gold", "%d", thing->gold);
        named_writef (stream, "hp", "%d", thing->hp);
        named_writef (stream, "dmg", "%d", thing->dmg);
        named_writef (stream, "speed", "%d", thing->speed);
        named_writef (stream, "equipped", "%d", thing->equipped);

        named_writef (stream, "remaining_wait", "%d", thing->remaining_wait);

        named_writef (stream, "behavior", "%d", (int) thing->behavior);
    }
}

/*
Reads the data for one thing from a stream.
*/
Thing
read_thing (FILE * stream)
{
    Thing thing = { 0 };

    int appearance = 0;
    readf (stream, "appearance", "%d", &appearance);
    thing.appearance = appearance;

    if (thing.appearance != DEAD)
    {
        read_str ("name", thing.name, NAME_SIZE, stream);
        readf (stream, "x", "%d", &thing.loc.x);
        readf (stream, "y", "%d", &thing.loc.y);

        readf (stream, "gold", "%d", &thing.gold);
        readf (stream, "hp", "%d", &thing.hp);
        readf (stream, "dmg", "%d", &thing.dmg);
        readf (stream, "speed", "%d", &thing.speed);

        int tmp = 0;
        readf (stream, "equipped", "%d", &tmp);
        thing.equipped = tmp != 0;

        readf (stream, "remaining_wait", "%d", &thing.remaining_wait);

        int behavior = 0;
        readf (stream, "behavior", "%d", &behavior);
        thing.behavior = behavior;
        thing.target = NULL;    // target is not saved
    }

    return thing;
}
