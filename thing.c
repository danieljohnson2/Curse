#include "thing.h"
#include "util.h"
#include "action.h"

#define BEHAVIOR_MAX 16

typedef struct ThingBehaviorDef
{
    BumpAction bump_action;
    TurnAction turn_action;
} ThingBehaviorDef;

static ThingBehaviorDef behaviorDefs[BEHAVIOR_MAX];

void
define_thing_behavior (ThingBehavior behavior, BumpAction bump_action,
                       TurnAction turn_action)
{
    behaviorDefs[behavior].bump_action = bump_action;
    behaviorDefs[behavior].turn_action = turn_action;
}

BumpAction
get_bump_action (Thing * thing)
{
    BumpAction action = behaviorDefs[thing->behavior].bump_action;

    if (action != NULL)
        return action;
    else
        return null_bump_action;
}

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
