#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

#include "defs.h"

Perlin make_perlin (double freq, int depth, int seed);
float perlin2d (Perlin * perlin, double x, double y);

#endif
