#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "perlin.h"
#include "loc.h"

#include <stdbool.h>

struct _Map;
typedef struct _Map Map;

typedef double (*MapShape) (Map * map, Loc where);

typedef enum _Terrain
{
    deep_sea,
    water,
    beach,
    grass,
    woods,
    hills,
    mountains
} Terrain;

struct _Map
{
    double soft_size;
    MapShape shape;
    Perlin perlin;
};

Map make_map (double soft_size, MapShape shape, Perlin perlin);
Terrain read_map (Map * map, Loc where);

int get_terrain_speed_penalty (Terrain terrain);
Loc find_passable_place (Map * map, Loc origin);

double round_shape (Map * map, Loc where);
double band_shape (Map * map, Loc where);

#endif
