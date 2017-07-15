#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "loc.h"
#include "perlin.h"
#include <stdio.h>

typedef struct Map Map;
typedef double (*MapShape) (Map * map, Loc where);

struct Map
{
    double soft_size;
    MapShape shape;
    Perlin perlin;
};

typedef enum _Terrain
{
    DEEP_SEA,
    WATER,
    BEACH,
    GRASS,
    WOODS,
    HILLS,
    MOUNTAINS
} Terrain;

Map make_map (double soft_size, MapShape shape, Perlin perlin);
Terrain read_map (Map * map, Loc where);

int get_terrain_speed_penalty (Terrain terrain);
Loc find_passable_place (Map * map, Loc origin);

char *get_shape_name (MapShape shape);
MapShape get_shape_from_name (char *shape_name);
double round_shape (Map * map, Loc where);
double band_shape (Map * map, Loc where);

void save_map (Map * map, FILE * stream);
Map restore_map (FILE * stream);

#endif
