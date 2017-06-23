#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

#include "perlin.h"

// brutally stolen from https://gist.github.com/nowl/828013

static int
get_hash (Perlin * perlin, long long index)
{
    index %= PERLIN_HASH_SIZE;

    if (index < 0 || index >= PERLIN_HASH_SIZE)
        abort ();

    return perlin->hash[index];
}

static int
noise2 (Perlin * perlin, long long x, long long y)
{
    int tmp = get_hash (perlin, y);
    return get_hash (perlin, tmp + x);
}

static float
lin_inter (double x, double y, double s)
{
    return x + s * (y - x);
}

static float
smooth_inter (double x, double y, double s)
{
    return lin_inter (x, y, s * s * (3 - 2 * s));
}

static float
noise2d (Perlin * perlin, double x, double y)
{
    long long x_int = x;
    long long y_int = y;
    double x_frac = x - x_int;
    double y_frac = y - y_int;
    int s = noise2 (perlin, x_int, y_int);
    int t = noise2 (perlin, x_int + 1, y_int);
    int u = noise2 (perlin, x_int, y_int + 1);
    int v = noise2 (perlin, x_int + 1, y_int + 1);
    double low = smooth_inter (s, t, x_frac);
    double high = smooth_inter (u, v, x_frac);
    return smooth_inter (low, high, y_frac);
}

Perlin
make_perlin (double freq, int depth, int seed)
{
    Perlin perlin = { 0 };
    perlin.freq = freq;
    perlin.depth = depth;

    srand (seed);

    for (int i = 0; i < PERLIN_HASH_SIZE; ++i)
        perlin.hash[i] = (rand () >> 8) & 0xFF;

    return perlin;
}

float
perlin2d (Perlin * perlin, double x, double y)
{
    static double offset = SHRT_MAX;

    double xa = abs (x + offset) * perlin->freq;
    double ya = abs (y + offset) * perlin->freq;
    double amp = 1.0;
    double fin = 0;
    double div = 0.0;

    int i;
    for (i = 0; i < perlin->depth; i++)
    {
        div += 256 * amp;
        fin += noise2d (perlin, xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin / div;
}
