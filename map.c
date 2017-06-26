#include "map.h"
#include "perlin.h"
#include "thing.h"
#include "util.h"

#include <stdlib.h>
#include <math.h>
#include <limits.h>

static double
pin (double value, double min, double max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

/* Creates a new map of the size and shape indicated. */
Map
make_map (double soft_size, MapShape shape, Perlin perlin)
{
    Map map = { 0 };
    map.soft_size = soft_size;
    map.shape = shape;
    map.perlin = perlin;
    return map;
}

#define TERRAIN_COUNT 19

/*
Decides what terrain to use at an altitude and
distance. Altitude is what we get from the perlin noise; distance
is defined by the shape.
*/
static Terrain
pick_terrain (Map * map, double altitude, double dist)
{
    Terrain a[TERRAIN_COUNT] = {
        DEEP_SEA,
        WATER,
        WATER,
        WATER,
        BEACH,
        GRASS,
        GRASS,
        GRASS,
        GRASS,
        WOODS,
        WOODS,
        WOODS,
        WOODS,
        GRASS,
        HILLS,
        HILLS,
        HILLS,
        MOUNTAINS,
        MOUNTAINS
    };

    double adjusted = altitude;

    if (dist > map->soft_size)
        dist -= map->soft_size;
    else if (dist < -map->soft_size)
        dist += map->soft_size;
    else
        dist = 0.0;

    adjusted += pin (dist / map->soft_size, -.65, .65);
    double unpinned = (adjusted * TERRAIN_COUNT - 1) + 0.5;
    int index = pin (unpinned, 0, TERRAIN_COUNT - 1);

    if (index < 0 || index >= TERRAIN_COUNT)
        abort ();

    return a[index];
}

/* Reads the terrain in a cell of the map. */
Terrain
read_map (Map * map, Loc where)
{
    double alt = perlin2d (&map->perlin, where.x, where.y);
    double dist = (map->shape) (map, where);
    return pick_terrain (map, alt, dist);
}

/*
Decides how slow movement through a terrain is; returns INT_MAX for
impassible terrain.
*/
int
get_terrain_speed_penalty (Terrain terrain)
{
    switch (terrain)
    {
    case WOODS:
        return SPEED_MAX / 2;
    case HILLS:
        return SPEED_MAX;

    case DEEP_SEA:
    case WATER:
    case MOUNTAINS:
        return INT_MAX;

    default:
        return 0;
    }
}

/*
Finds a place that has passable terrain in the map.
*/
Loc
find_passable_place (Map * map, Loc origin)
{
    int size = map->soft_size;

    for (;;)
    {
        Loc c;
        c.x = (rand () % size) - size / 2;
        c.y = (rand () % size) - size / 2;
        c.x += isign (c.x) * size / 2;
        c.y += isign (c.y) * size / 2;
        c.x += origin.x;
        c.y += origin.y;

        Terrain t = read_map (map, c);

        if (get_terrain_speed_penalty (t) < INT_MAX)
            return c;
    }
}

/* Defines a map shape that is circular. */
double
round_shape (Map * map, Loc where)
{
    return map->soft_size * 1.5 - sqrt (where.x * where.x +
                                        where.y * where.y);
}

/*
Defines a map shape that is a diagonal band with sea on the left,
and mountains on the right.
*/
double
band_shape (Map * map, Loc where)
{
    return (where.x + where.y) / 2.0;
}
