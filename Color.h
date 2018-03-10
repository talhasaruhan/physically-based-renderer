#ifndef COLOR_H
#define COLOR_H

#include <algorithm>

enum class colorSpace { RGB, HSV, HSL };

class Color4dHSL;

class Color4dRGB {
private:
    void update(double, colorSpace);
    void update(double, double, double, colorSpace);

public:
    double r, g, b, a;

    Color4dRGB();
    Color4dRGB(const Color4dRGB&);
    Color4dRGB(const Color4dHSL&);
    Color4dRGB(double x, colorSpace k = colorSpace::RGB);
    Color4dRGB(double x, double y, double z, colorSpace k = colorSpace::RGB);
    Color4dRGB(double x, double y, double z, double w, colorSpace k = colorSpace::RGB);

    void set_Alpha(double);
    void set_RGB(double, double, double);
    void set_HSV(double, double, double);
    void set_HSL(double, double, double);

    Color4dRGB gamma_corrected() const;
    void gamma_correct();

    Color4dRGB operator/(const double& b);
    Color4dRGB operator*(const Color4dRGB& b);
    Color4dRGB operator*(const double& b);
    Color4dRGB operator+(const Color4dRGB& b);

    static inline double _gamma_correction(double);
    
    ~Color4dRGB();
};

class Color4dHSL {
private:
    void update(double, colorSpace);
    void update(double, double, double, colorSpace);

public:
    double h, s, l, a;

    Color4dHSL();
    Color4dHSL(const Color4dHSL&);
    Color4dHSL(const Color4dRGB&);
    Color4dHSL(double x, colorSpace k = colorSpace::HSL);
    Color4dHSL(double x, double y, double z, colorSpace k = colorSpace::HSL);
    Color4dHSL(double x, double y, double z, double w, colorSpace k = colorSpace::HSL);

    void set_Alpha(double);
    void set_RGB(double, double, double);
    void set_HSV(double, double, double);
    void set_HSL(double, double, double);

    Color4dHSL operator/(const double& b);
    Color4dHSL operator*(const Color4dHSL& b);
    Color4dHSL operator*(const double& b);
    Color4dHSL operator+(const Color4dHSL& b);

    ~Color4dHSL();
};

double* HSV_to_RGB(double, double, double);
double* HSV_to_HSL(double, double, double);

double* RGB_to_HSV(double, double, double);
double* RGB_to_HSL(double, double, double);

double* HSL_to_RGB(double, double, double);
double* HSL_to_HSV(double, double, double);
#endif
