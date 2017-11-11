#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

#define PERLIN_HASH_SIZE 4096

#include <stdio.h>

typedef struct Perlin
{
    double freq;
    int depth;
    double origin_offset;

    unsigned char hash[PERLIN_HASH_SIZE];
} Perlin;

Perlin make_perlin (double freq, int depth, int seed);
double perlin2d (Perlin * perlin, double x, double y);

void write_perlin (Perlin * perlin, FILE * stream);
Perlin read_perlin (FILE * stream);

#endif
