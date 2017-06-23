#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "perlin.h"

#include <stdbool.h>

struct _Map;
typedef struct _Map Map;

typedef double (*MapShape) (Map * map, int x, int y);

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
Terrain read_map (Map * map, int x, int y);
bool is_passable (Terrain terrain);

double round_shape (Map * map, int x, int y);
double band_shape (Map * map, int x, int y);

#endif
