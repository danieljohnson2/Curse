#include "map.h"
#include "action.h"
#include "util.h"

#include <string.h>
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
        message_abort ("Invalid terrain index");

    return a[index];
}

/* Reads the terrain in a cell of the map. */
Terrain
get_map_terrain (Map * map, Loc where)
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

        Terrain t = get_map_terrain (map, c);

        if (get_terrain_speed_penalty (t) < INT_MAX)
            return c;
    }
}

/*
This writes a map structure to a file; this converts
the MapShape function to its name so it can be safely restored.
*/
void
write_map (Map * map, FILE * stream)
{
    named_writef (stream, "soft_size", "%lf", map->soft_size);
    named_writef (stream, "shape", "%s", get_shape_name (map->shape));
    write_perlin (&map->perlin, stream);
}

/*
This restores a map from its file. Since we write the
map at the end of the file, this function depends upon
the stream being in the correct position, where it
was when save_map() was called.
*/
Map
read_map (FILE * stream)
{
    Map map = { 0 };
    readf (stream, "soft_size", "%lf", &map.soft_size);

    char shape_name[PATH_MAX] = { 0 };
    read_str ("shape", shape_name, PATH_MAX, stream);
    map.shape = get_shape_from_name (shape_name);

    map.perlin = read_perlin (stream);
    return map;
}
