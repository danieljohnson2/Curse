#include "perlin.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

// Borowed (with some rework) from https://gist.github.com/nowl/828013
// Thanks nowl!

/*
This retrieves a hash value from the perlin struct given its index;
but this verify the index (as I've had some problems with that).
*/
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

/*
Creates a perlin structure. The hash data is randly generated based on
the seed given (so the same seed will always produce the same data).
*/
Perlin
make_perlin (double freq, int depth, int seed)
{
    Perlin perlin = { 0 };
    perlin.freq = freq;
    perlin.depth = depth;
    perlin.origin_offset = SHRT_MAX;

    srand (seed);

    // rand() is crap, but who cares, this is just a wee game.
    for (int i = 0; i < PERLIN_HASH_SIZE; ++i)
        perlin.hash[i] = rand () & 0xFF;

    return perlin;
}

/*
Reads a value from the perlin noise at the co-ordinates given.
This tolerates negative co-ordinates, but just mirrors the nosie
at the origin. To make this less obvious, you can have an origin-offset
that effectively moves this origin out of the way. 
*/
float
perlin2d (Perlin * perlin, double x, double y)
{
    double xa = abs (x + perlin->origin_offset) * perlin->freq;
    double ya = abs (y + perlin->origin_offset) * perlin->freq;
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
