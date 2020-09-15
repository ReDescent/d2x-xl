
#include <math.h>
#include <stdlib.h>

#include "SimplexNoise.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

double CSimplexNoise::Noise(double v) {
    int32_t i = int32_t(FastFloor(v)) & 255;
    v -= FastFloor(v);
    double u = Fade(v);
#if DBG
    double a = Grad(m_random[i], v);
    double b = Grad(m_random[i + 1], v - 1);
    double l = Lerp(u, a, b);
    return l;
#else
    return Lerp(u, Grad(m_random[i], v), Grad(m_random[i + 1], v - 1));
#endif
}

//------------------------------------------------------------------------------

double CSimplexNoise::Noise(double x, double y) {
    int32_t xi = (int32_t)FastFloor(x);
    int32_t yi = (int32_t)FastFloor(y);
    x -= (double)xi;
    y -= (double)yi;
    xi &= 255;
    yi &= 255;
    double u = Fade(x);
    double v = Fade(y);
    int32_t A = m_random[xi] + yi;
    int32_t B = m_random[xi + 1] + yi;
    return Lerp(
        v,
        Lerp(u, Grad(m_random[A], x, y), Grad(m_random[B], x - 1, y)),
        Lerp(u, Grad(m_random[A + 1], x, y - 1), Grad(m_random[B + 1], x - 1, y - 1)));
}

//------------------------------------------------------------------------------

double CSimplexNoise::Grad(int32_t bias, double x, double y, double z) {
    int32_t h = bias & 15;
    double u = (h < 8) ? x : y;
    double v = (h < 4) ? y : ((h == 12) || (h == 14)) ? x : z;
    return (((h & 1) == 0) ? u : -u) + (((h & 2) == 0) ? v : -v);
}

//------------------------------------------------------------------------------

void CSimplexNoise::Setup(double amplitude, double persistence, int32_t octaves, int32_t randomize) {
    CPerlinNoise::Setup(amplitude * 2.0, persistence, octaves, 0);
    int32_t i, permutation[PERLIN_RANDOM_SIZE];

    for (i = 0; i < PERLIN_RANDOM_SIZE; i++)
        permutation[i] = i;
    int32_t l = PERLIN_RANDOM_SIZE;

    int32_t *p = m_random + PERLIN_RANDOM_SIZE;
    memcpy(p, permutation, sizeof(permutation));
    for (int32_t i = 0; i < PERLIN_RANDOM_SIZE; i++) {
        int32_t j = (int32_t)Rand(l);
        m_random[i] = p[j];
        if (j < --l)
            p[j] = p[l];
    }
    memcpy(p, m_random, sizeof(permutation));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
