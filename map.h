#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "defs.h"

Map make_map (double soft_size, MapShape shape, Perlin perlin);
Terrain read_map (Map * map, Loc where);

int get_terrain_speed_penalty (Terrain terrain);
Loc find_passable_place (Map * map, Loc origin);

double round_shape (Map * map, Loc where);
double band_shape (Map * map, Loc where);

#endif
