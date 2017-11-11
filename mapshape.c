#include "map.h"

#include <string.h>
#include <math.h>

/* Defines a map shape that is circular. */
static double
round_shape (Map * map, Loc where)
{
    return map->soft_size * 1.5 - sqrt (where.x * where.x +
                                        where.y * where.y);
}

/*
Defines a map shape that is a diagonal band with sea on the left,
and mountains on the right.
*/
static double
band_shape (Map * map, Loc where)
{
    (void) map;                 // not used
    return (where.x + where.y) / 2.0;
}

static struct
{
    char *name;
    MapShape shape;
} named_map_shapes[] =
{
    {
    "round", round_shape},
    {
    "band", band_shape},
    {
    NULL, NULL}
};

/*
Returns the name of the shape indicated;
this is the inverse of get_shape_from_name.
*/
char *
get_shape_name (MapShape shape)
{
    for (int i = 0;; ++i)
    {
        char *name = named_map_shapes[i].name;
        if (name == NULL)
            return NULL;
        else if (named_map_shapes[i].shape == shape)
            return name;
    }
}

/*
Resolves a shape name (used in the command line arguments) to
the shape function. Returns null if the name is not valid.
*/
MapShape
get_shape_from_name (char *shape_name)
{
    for (int i = 0;; ++i)
    {
        char *name = named_map_shapes[i].name;
        if (name == NULL)
            return NULL;
        else if (strcmp (name, shape_name) == 0)
            return named_map_shapes[i].shape;
    }
}
