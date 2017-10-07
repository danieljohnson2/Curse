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

bool
weapon_pickup_bump_action (Thing * actor, Thing * target)
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

    return true;
}
