#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

#define PERLIN_HASH_SIZE 4096

typedef struct _PERLIN
{
    double freq;
    int depth;

    unsigned char hash[PERLIN_HASH_SIZE];
} PERLIN;

void init_perlin (PERLIN * perlin, double freq, int depth, int seed);
float perlin2d (PERLIN * perlin, double x, double y);

#endif
