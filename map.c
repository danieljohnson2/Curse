#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "map.h"

Map
make_map (double soft_size, MapShape shape, Perlin perlin)
{
    Map map = { 0 };
    map.soft_size = soft_size;
    map.shape = shape;
    map.perlin = perlin;
    return map;
}

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

#define TERRAIN_COUNT 19

static Terrain
pick_terrain (Map * map, double depth, double dist)
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

    double adjusted = depth;

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
        printf ("Bad depth: %f", depth);
        abort ();
    }

    return a[index];
}

Terrain
read_map (Map * map, int x, int y)
{
    double p = perlin2d (&map->perlin, x, y);
    double dist = (map->shape) (map, x, y);
    return pick_terrain (map, p, dist);
}

double
round_shape (Map * map, int x, int y)
{
    return map->soft_size * 1.5 - sqrt (x * x + y * y);
}

double
band_shape (Map * map, int x, int y)
{
    return (x + y) / 2.0;
}
