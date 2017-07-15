#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

#define PERLIN_HASH_SIZE 4096

typedef struct Perlin
{
    double freq;
    int depth;
    double origin_offset;

    unsigned char hash[PERLIN_HASH_SIZE];
} Perlin;

Perlin make_perlin (double freq, int depth, int seed);
float perlin2d (Perlin * perlin, double x, double y);

#endif
