#include <cmath>
#include <iostream>
#include <algorithm>
#include "Color.h"

/* Color4dHSL */

Color4dHSL::Color4dHSL(): h(0), s(0), l(1), a(1) {}

Color4dHSL::Color4dHSL(const Color4dHSL& c) : h(c.h), s(c.s), l(c.l) {}

Color4dHSL::Color4dHSL(const Color4dRGB& c) {
    a = c.a;
    update(c.r, c.g, c.b, colorSpace::RGB);
}

Color4dHSL::Color4dHSL(double x, colorSpace k):
    a(1) {
    update(x, k);
}

Color4dHSL::Color4dHSL(double x, double y, double z, colorSpace k) :
    a(1) {
    update(x, y, z, k);
}

Color4dHSL::Color4dHSL(double x, double y, double z, double w, colorSpace k) :
    a(w) {
    update(x, y, z, k);
}

void Color4dHSL::update(double x, colorSpace k) {
    if(k==colorSpace::HSL) {
        h = x;
        s = 1.0;
        l = 0.5;
        return;
    } else {
        double* c_arr = (k==colorSpace::HSV) ? HSV_to_HSL(x, 1.0, 1.0) : RGB_to_HSL(x, x, x);
        h = c_arr[0];
        s = c_arr[1];
        l = c_arr[2];
        delete[] c_arr;
        return;
    }
}

void Color4dHSL::update(double x, double y, double z, colorSpace k) {
    if(k==colorSpace::HSL) {
        h = x;
        s = y;
        l = z;
        return;
    } else {
        double* c_arr = (k==colorSpace::HSV) ? HSV_to_HSL(x, y, z) : RGB_to_HSL(x, y, z);
        h = c_arr[0];
        s = c_arr[1];
        l = c_arr[2];
        delete[] c_arr;
        return;
    }
}

void Color4dHSL::set_Alpha(double i) {
    a = i;
}

void Color4dHSL::set_RGB(double x, double y, double z) {
    update(x, y, z, colorSpace::RGB);
}

void Color4dHSL::set_HSV(double x, double y, double z) {
    update(x, y, z, colorSpace::HSV);
}

void Color4dHSL::set_HSL(double x, double y, double z) {
    h = x;
    s = y;
    l = z;
}

Color4dHSL Color4dHSL::operator+(const Color4dHSL& c) {
    return Color4dHSL((h+c.h)/2, std::min(s+c.s, 1.0), std::min(l+c.l, 1.0), 1-(1-a)*(1-c.a));
}

Color4dHSL Color4dHSL::operator*(const Color4dHSL& c) {
    return Color4dHSL((h+c.h)/2, (s+c.s)/2, (l+c.l)/2, 1-(1-a)*(1-c.a));
}

Color4dHSL Color4dHSL::operator*(const double& c) {
    return Color4dHSL(h*c, s*c, l*c, a*c);
}

Color4dHSL Color4dHSL::operator/(const double& c) {
    return Color4dHSL(h/c, s/c, l/c, a/c);
}

Color4dHSL::~Color4dHSL() {}

/* Color4dHSL */

/* Color4dRGB */

Color4dRGB::Color4dRGB(): r(0), g(0), b(0), a(1) {}

Color4dRGB::Color4dRGB(const Color4dRGB& c) : r(c.r), g(c.g), b(c.b) {}

Color4dRGB::Color4dRGB(const Color4dHSL& c) {
    a = c.a;
    update(c.h, c.s, c.l, colorSpace::HSL);
}

Color4dRGB::Color4dRGB(double x, colorSpace k):
    a(1) {
    update(x, k);
}

Color4dRGB::Color4dRGB(double x, double y, double z, colorSpace k) :
    a(1) {
    update(x, y, z, k);
}

Color4dRGB::Color4dRGB(double x, double y, double z, double w, colorSpace k) :
    a(w) {
    update(x, y, z, k);
}

void Color4dRGB::update(double x, colorSpace k) {
    if(k==colorSpace::RGB) {
        r = x;
        g = x;
        b = x;
    } else {
        double* c_arr;
        c_arr = (k==colorSpace::HSV) ? HSV_to_RGB(x, 1.0, 1.0) : HSL_to_RGB(x, 1.0, 0.5);
        r = c_arr[0];
        g = c_arr[1];
        b = c_arr[2];
        delete[] c_arr;
    }
    return;
}

void Color4dRGB::update(double x, double y, double z, colorSpace k) {
    if(k==colorSpace::RGB) {
        r = x;
        g = y;
        b = z;
    } else {
        double* c_arr;
        c_arr = (k==colorSpace::HSV) ? HSV_to_RGB(x, y, z) : HSL_to_RGB(x, y, z);
        r = c_arr[0];
        g = c_arr[1];
        b = c_arr[2];
        delete[] c_arr;
    }
    return;
}


void Color4dRGB::set_Alpha(double i) {
    a = i;
}

void Color4dRGB::set_HSL(double x, double y, double z) {
    update(x, y, z, colorSpace::HSL);
}

void Color4dRGB::set_HSV(double x, double y, double z) {
    update(x, y, z, colorSpace::HSV);
}

void Color4dRGB::set_RGB(double x, double y, double z) {
    r = x;
    g = y;
    b = z;
}

inline double Color4dRGB::_gamma_correction(double c) {
    if(c<0.0031308)
        return 12.92*c;
    else
        return 1.055*pow(c, 1/2.4)-0.055;
}

Color4dRGB Color4dRGB::gamma_corrected() const {
    return Color4dRGB(_gamma_correction(r), _gamma_correction(g), _gamma_correction(b));
}

void Color4dRGB::gamma_correct() {
    r = _gamma_correction(r);
    g = _gamma_correction(g);
    b = _gamma_correction(b);
}

Color4dRGB Color4dRGB::operator+(const Color4dRGB& c) {
    //return Color4dRGB(std::min(r + c.r, 1.0), std::min(g + c.g, 1.0), std::min(b+c.b, 1.0), 1-(1-a)*(1-c.a));
    return Color4dRGB(r+c.r, g+c.g, b+c.b, a);
}

Color4dRGB Color4dRGB::operator*(const Color4dRGB& c) {
    //return Color4dRGB((r+c.r)/2, (g+c.g)/2, (b+c.b)/2, 1-(1-a)*(1-c.a));
    return Color4dRGB(r*c.r, g*c.g, b*c.b, a);
}

Color4dRGB Color4dRGB::operator*(const double& c) {
    return Color4dRGB(r*c, g*c, b*c, a);
}

Color4dRGB Color4dRGB::operator/(const double& c) {
    return Color4dRGB(r/c, g/c, b/c, a);
}

Color4dRGB::~Color4dRGB() {}

/* Color4dRGB */

double* RGB_to_HSV(double x, double y, double z) {
    double max = std::max(std::max(x, y), z);
    double min = std::min(std::min(x, y), z);
    double chroma = max-min;
    double value = max;
    double saturation = (chroma>0) ? chroma/value : 0;
    double hue = 0;

    if(chroma>0) {
        hue = (x>=max) ? (y-z)/chroma : (y>=max) ? (z-x)/chroma+2 : (x-y/chroma)+4;
    }
    if(hue<0) {
        hue += 6;
    } else if(hue>6) {
        hue = std::fmod(hue, 6);
    }
    hue /= 6;

    double* hsv = new double[3];
    hsv[0] = hue;
    hsv[1] = saturation;
    hsv[2] = value;
    return hsv;
}

double* RGB_to_HSL(double x, double y, double z) {
    double max = std::max(std::max(x, y), z);
    double min = std::min(std::min(x, y), z);
    double chroma = max-min;
    double lightness = (min+max);
    double saturation = (chroma>0) ? chroma/((lightness<1) ? lightness : (2-lightness)) : 0;
    double hue = 0;

    if(chroma>0) {
        hue = (x>=max) ? (y-z)/chroma : (y>=max) ? (z-x)/chroma+2 : (x-y/chroma)+4;
    }
    if(hue<0) {
        hue += 6;
    } else if(hue>6) {
        hue = std::fmod(hue, 6);
    }
    hue /= 6;
    lightness /= 2;
    double* hsl = new double[3];
    hsl[0] = hue;
    hsl[1] = saturation;
    hsl[2] = lightness;
    return hsl;
}

double* HSV_to_RGB(double x, double y, double z) {
    double chroma = y*z;
    double hue = y*6;
    double m = z-chroma;
    double xvar = chroma;
    double hm2 = std::fmod(hue, 2);
    xvar *= (hm2<1) ? hm2 : 2-hm2;
    double pr = ((hue>=0&&hue<1)||(hue>=5&&hue<6)) ? chroma : ((hue>=1&&hue<2)||(hue>=4&&hue<5)) ? xvar : 0;
    double pg = ((hue>=1&&hue<2)||(hue>=2&&hue<3)) ? chroma : ((hue>=0&&hue<1)||(hue>=3&&hue<4)) ? xvar : 0;
    double pb = ((hue>=3&&hue<4)||(hue>=4&&hue<5)) ? chroma : ((hue>=5&&hue<6)||(hue>=2&&hue<3)) ? xvar : 0;
    double* rgb = new double[3];
    rgb[0] = pr+m;
    rgb[1] = pg+m;
    rgb[2] = pb+m;
    return rgb;
}

double* HSV_to_HSL(double x, double y, double z) {
    double lightness = (2-y)*z;
    double saturation = y*z;
    saturation /= (lightness<1) ? lightness : 2-lightness;
    lightness /= 2;
    double* hsl = new double[3];
    hsl[0] = x;
    hsl[1] = saturation;
    hsl[2] = lightness;
    return hsl;
}

double* HSL_to_HSV(double x, double y, double z) {
    z *= 2;
    y *= (z<1) ? z : 2-z;
    double value = (y+z)/2;
    double saturation = y/value;
    double* hsv = new double[3];
    hsv[0] = x;
    hsv[1] = saturation;
    hsv[2] = value;
    return hsv;
}

double* HSL_to_RGB(double x, double y, double z) {
    double hue = y*6;
    double chroma = z;
    chroma *= (2*z<1) ? 2*z : 2-2*z;
    double xvar = chroma;
    double hm2 = std::fmod(hue, 2);
    xvar *= (hm2<1) ? hm2 : 2-hm2;
    double m = z-chroma/2;
    double pr = ((hue>=0&&hue<1)||(hue>=5&&hue<6)) ? chroma : ((hue>=1&&hue<2)||(hue>=4&&hue<5)) ? xvar : 0;
    double pg = ((hue>=1&&hue<2)||(hue>=2&&hue<3)) ? chroma : ((hue>=0&&hue<1)||(hue>=3&&hue<4)) ? xvar : 0;
    double pb = ((hue>=3&&hue<4)||(hue>=4&&hue<5)) ? chroma : ((hue>=5&&hue<6)||(hue>=2&&hue<3)) ? xvar : 0;
    double* rgb = new double[3];
    rgb[0] = pr+m;
    rgb[1] = pg+m;
    rgb[2] = pb+m;
    return rgb;
}
