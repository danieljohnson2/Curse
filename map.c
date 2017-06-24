#include "map.h"
#include "thing.h"

#include <stdio.h>
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
        deep_sea,
        water,
        water,
        water,
        beach,
        grass,
        grass,
        grass,
        grass,
        woods,
        woods,
        woods,
        woods,
        grass,
        hills,
        hills,
        hills,
        mountains,
        mountains
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
    {
        printf ("Bad terrain index: %d", index);
        abort ();
    }

    return a[index];
}

/* Reads the terrain in a cell of the map. */
Terrain
read_map (Map * map, int x, int y)
{
    double alt = perlin2d (&map->perlin, x, y);
    double dist = (map->shape) (map, x, y);
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
    case woods:
        return SPEED_MAX / 2;
    case hills:
        return SPEED_MAX;

    case deep_sea:
    case water:
    case mountains:
        return INT_MAX;

    default:
        return 0;
    }
}

/* Defines a map shape that is circular. */
double
round_shape (Map * map, int x, int y)
{
    return map->soft_size * 1.5 - sqrt (x * x + y * y);
}

/*
Defines a map shape that is a diagonal band with sea on the left,
and mountains on the right.
*/
double
band_shape (Map * map, int x, int y)
{
    return (x + y) / 2.0;
}
