#include "MathUtils.h"

unsigned long wrng512_state[16];
unsigned int wrng512_index = 0;

int64_t xorshift64_x = 0;

std::random_device rd;
std::mt19937 mt(rd());

double lerp3d(double x, double y, double t) {
    return x+(y-x)*t;
}

void seed() {
    init_xorshift64();
    init_wrng512();
}

double random_d(double x, double y) {
    return x+(y-x)*xorshift64d();
}

uint64_t xorshift64uint64(int64_t& x) {
    x ^= x>>12;
    x ^= x<<25;
    x ^= x>>27;

    return x*2685821657736338717ull;
}

double xorshift64d(int64_t& x) {
    return int64_t(xorshift64uint64(x))*POW2_M64+0.5;
}

void init_xorshift64() {
    xorshift64_x = (uint64_t)rd();
}

uint32_t wellrng512uint32(unsigned long* state, unsigned int& index) {
    unsigned long a, b, c, d;
    a = state[index];
    c = state[(index+13)&15];
    b = a^c^(a<<16)^(c<<15);
    c = state[(index+9)&15];
    c ^= (c>>11);
    a = state[index] = b^c;
    d = a^((a<<5)&0xDA442D24UL);
    index = (index+15)&15;
    a = state[index];
    state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
    return state[index];
}

double wellrng512d(unsigned long* state, unsigned int& index) {
    return wellrng512uint32(state, index)*POW2_M32;
}

void init_wrng512() {
    for(int i = 0; i<16; ++i)
        wrng512_state[i] = rd();
    wrng512_index = 0;
}
