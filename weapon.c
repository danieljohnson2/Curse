#include "weapon.h"
#include "message.h"
#include "game.h"

#include <stdlib.h>

#define CANDIDATE_WEAPON_COUNT 3

typedef struct WeaponData
{
    char *name;
    int dmg;
} WeaponData;

static WeaponData weapon_data[CANDIDATE_WEAPON_COUNT] = {
    {"Dagger", 10},
    {"Mace", 15},
    {"Sword", 20}
};

static Thing
make_weapon (WeaponData data)
{
    Thing monster = make_thing (WEAPON, data.name, 0, WEAPON_PICKUP);
    monster.dmg = data.dmg;
    return monster;
}

/* Constructs a random weapon thing */
Thing
make_random_weapon (void)
{
    static bool initialized;
    static Thing candidates[CANDIDATE_WEAPON_COUNT];

    if (!initialized)
    {
        initialized = true;

        for (int i = 0; i < CANDIDATE_WEAPON_COUNT; ++i)
            candidates[i] = make_weapon (weapon_data[i]);
    }

    int index = rand () % CANDIDATE_WEAPON_COUNT;
    return candidates[index];
}

/* Finds the equipped item belonging to 'owner' whose behavior matches
thing_type, and returns it. If no such itme is equipped, this returns NULL. */
Thing *
get_equipped_item (Thing * owner, ThingBehavior thing_type)
{
    for (Thing * th = NULL; next_thing (owner, &th);)
    {
        if (th->equipped && th->behavior == thing_type)
            return th;
    }

    return NULL;
}

/* Equips an item. Any other item owned by 'owner' of the same type is
unequipped. */
void
equip_item (Thing * owner, Thing * item)
{
    for (Thing * th = NULL; next_thing (owner, &th);)
    {
        if (th->behavior == item->behavior)
            th->equipped = th == item;
    }
}

static Thing *
weapon_pickup_bump_action_core (Thing * actor, Thing * target)
{
    if (inventory_contains (actor, *target))
    {
        write_messagef ("%s picked up %s (again)!", actor->name,
                        target->name);
        remove_thing (target);
        return NULL;
    }
    else
    {
        Thing *carried = copy_to_inventory (actor, *target);

        if (carried != NULL)
        {
            write_messagef ("%s picked up %s!", actor->name, target->name);
            remove_thing (target);
        }
        else
        {
            write_messagef ("%s can't pick up any more!", actor->name);
        }

        return carried;
    }
}

bool
weapon_pickup_bump_action (Thing * actor, Thing * target)
{
    if (actor->behavior == ANIMAL)
        return true;

    Thing *to_equip = weapon_pickup_bump_action_core (actor, target);

    if (actor->behavior != PLAYER_CONTROLLED && to_equip != NULL)
    {
        if (actor->behavior == SMART_MONSTER)
        {
            for (Thing * th = NULL; next_thing (actor, &th);)
                if (to_equip->dmg < th->dmg)
                    to_equip = th;
        }

        equip_item (actor, to_equip);
    }

    return true;
}

bool
weapon_dumb_pickup_bump_action (Thing * actor, Thing * target)
{
    Thing *gotten = weapon_pickup_bump_action_core (actor, target);

    // equip what actor picked up, no matter what it is!
    for (Thing * th = NULL; next_thing (actor, &th);)
        th->equipped = th == gotten;

    return true;
}
