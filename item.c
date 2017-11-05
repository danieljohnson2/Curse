#include "item.h"
#include "game.h"
#include "message.h"
#include "game.h"

#include <stdlib.h>

#define CANDIDATE_ITEM_COUNT 7

typedef struct ItemData
{
    Appearance appearance;
    char *name;
    int dmg;
    int hp;
    ThingBehavior behavior;
} ItemData;

static ItemData item_data[CANDIDATE_ITEM_COUNT] = {
    {POTION, "Healing Potion", 0, 10, HEALING_POTION},
    {WEAPON, "Dagger", 10, 0, WEAPON_PICKUP},
    {ARMOR, "Leather Armor", 3, 0, ARMOR_PICKUP},
    {WEAPON, "Mace", 15, 0, WEAPON_PICKUP},
    {ARMOR, "Chain Armor", 10, 0, ARMOR_PICKUP},
    {WEAPON, "Sword", 20, 0, WEAPON_PICKUP},
    {ARMOR, "Plate Armor", 15, 0, ARMOR_PICKUP}
};

static Thing
make_item (ItemData data)
{
    Thing item = make_thing (data.appearance, data.name, 0, data.behavior);
    item.dmg = data.dmg;
    item.max_hp = data.hp;
    item.hp = data.hp;
    return item;
}

/* Constructs a random item, which could be weapon or armor, taken
from the item_data. */
Thing
make_random_item (void)
{
    static bool initialized;
    static Thing candidates[CANDIDATE_ITEM_COUNT];

    if (!initialized)
    {
        initialized = true;

        for (int i = 0; i < CANDIDATE_ITEM_COUNT; ++i)
            candidates[i] = make_item (item_data[i]);
    }

    int index = rand () % CANDIDATE_ITEM_COUNT;
    return candidates[index];
}

/* Checks to see if the owner given conains a (copy of)
The thing given. */
bool
inventory_contains (Thing * owner, Thing thing)
{
    for (Thing * th = NULL; next_thing (owner, &th);)
    {
        if (equal_things (*th, thing))
            return true;
    }

    return false;
}

/* Adds a (copy of) thing to the inventory of an owner, if there's
space. Returns a pointer to the inventory copy, or NULL if there's
no room. */
Thing *
copy_to_inventory (Thing * owner, Thing thing)
{
    for (Thing * th = NULL; next_possible_thing (owner, &th);)
    {
        if (!is_thing_alive (th))
        {
            *th = thing;
            return th;
        }
    }

    return NULL;
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

/* Picks up the target item and places it in the actors inventory,
if possible. Returns the thing now in inventory, or NULL if the
actor could not pick it up because its inventory was full. */
static Thing *
item_pickup_bump_action_core (Thing * actor, Thing * target)
{
    if (target->behavior != HEALING_POTION
        && inventory_contains (actor, *target))
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

/* Handles the case where something picks up an item; this decides
whether the actor will equip the item, which depends on the behavior
of the actor. */
bool
item_pickup_bump_action (Thing * actor, Thing * target)
{
    if (actor->behavior == ANIMAL)
        return true;

    Thing *picked_up = item_pickup_bump_action_core (actor, target);
    if (picked_up != NULL)
    {
        UseAction use_action = get_use_action (picked_up);

        if (use_action != NULL)
        {
            if (actor->appearance != PLAYER)
                use_action (picked_up, actor);
        }
        else
        {
            if (actor->behavior != PLAYER_CONTROLLED && picked_up != NULL)
            {
                for (Thing * th = NULL; next_thing (actor, &th);)
                    if (th->behavior == picked_up->behavior
                        && picked_up->dmg < th->dmg)
                        picked_up = th;

                equip_item (actor, picked_up);
            }
        }
    }

    return true;
}

/* Handles drinking a healing potion. */
void
healing_potion_use_action (Thing * potion, Thing * user)
{
    int new_hp = user->hp + potion->hp;
    if (new_hp > user->max_hp)
        new_hp = user->max_hp;

    remove_thing (potion);

    if (new_hp > user->hp)
    {
        write_messagef ("%s drinks a potion and recovers %d hp!", user->name,
                        new_hp - user->hp);
        user->hp = new_hp;
    }
    else
        write_messagef ("%s drinks a potion but nothing happens!",
                        user->name);
}
