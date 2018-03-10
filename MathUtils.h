#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "stdint.h"
#include <cmath>
#include <time.h>
#include <random>
#include <iostream>
#include <algorithm>

#define POW2_M32   2.3283064365386962890625e-010
#define POW2_M64   5.421010862427522170037264004349e-020

// init to random variables
extern unsigned long wrng512_state[16];
// init to 0
extern unsigned int wrng512_index;

extern int64_t xorshift64_x;

double lerp3d(double, double, double);

double random_d(double, double);

void seed();
void init_xorshift64();
void init_wrng512();

uint64_t xorshift64uint64(int64_t& x = xorshift64_x);
uint32_t wellrng512uint32(unsigned long* state = &wrng512_state[0], unsigned int& index = wrng512_index);

double xorshift64d(int64_t& x = xorshift64_x); // [0, 1)
double wellrng512d(unsigned long* state = &wrng512_state[0], unsigned int& index = wrng512_index); // [0, 1)

#endif
