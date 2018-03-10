#include "GGX.h"

const double PI = 3.14;

thread_local std::mt19937 GGX::generator = std::mt19937(time(NULL));
thread_local std::uniform_real_distribution<double> GGX::distribution = std::uniform_real_distribution<double>(0.0, 1.0);

GGX::GGX(double alpha, Color4dRGB c, double n): roughness(alpha), color(c), ior(n) {}

double GGX::D(const Vector3d& wi, const Vector3d& wo, const Vector3d& m, const Vector3d& n) {
    double D, t = n.dot(m), a = roughness*roughness;
    if(t<0) {
        D = 0;
    } else {
        t = 1+t*t*(a-1);
        t = t*t;
        D = a/(PI*t);
    }
    return D;
}

inline double GGX::G1(const Vector3d& w, const Vector3d& m, const Vector3d& n) {
    double G, tg2_v;

    if(w.dot(m)/w.dot(n)<0) {
        G = 0;
    } else {
        tg2_v = w.dot(n);
        tg2_v = tg2_v*tg2_v;
        tg2_v = (1-tg2_v)/tg2_v;
        G = 2/(1+sqrt(1+roughness*roughness*tg2_v));
    }

    return G;
}

double GGX::G(const Vector3d& wo, const Vector3d& wi, const Vector3d& m, const Vector3d& n) {
    return G1(wo, m, n)*G1(wi, m, n);
}

double GGX::F(const Vector3d& wo, const Vector3d& wi, const Vector3d& m) {
    double F0 = (1-ior)/(1+ior);
    F0 = F0*F0;
    return F0+(1-F0)*pow(1-m.dot(wo), 4);
}


double GGX::brdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    Vector3d m = (wi+wo).normalized();
    return D(wo, wi, m, n)*F(wo, wi, n)*G(wo, wi, m, n)/(4*wo.dot(n)*wi.dot(n));
}

double GGX::pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    Vector3d m = (wi+wo).normalized();
    return D(wo, wi, m, n)*m.dot(n)/(4*m.dot(wi));
}

Color4dRGB GGX::explicit_eval(const Vector3d& wo, const Vector3d& wi, const Vector3d& n) {
    return n.dot(wi)*brdf(-wo, wi, n);
}

Vector3d GGX::importance_sampling(const Vector3d& wo, const Vector3d& n) {
    return sample_microfacet_normal(n);
}

inline Vector3d GGX::sample_microfacet_normal(const Vector3d& n) {
    double r1 = distribution(generator), r2 = distribution(generator);
    double phi = 2*PI*r2;
    double sin_theta = roughness*sqrt(r1/(1+r1*(roughness*roughness-1)))*cos(phi);
    double cos_theta = sqrt((1-r1)/(1+r1*(roughness*roughness-1)));

    // first sample microfacet normal wrt surface normal
    Vector3d m = Vector3d(sin_theta*cos(phi), sin_theta*sin(phi), cos_theta);

    //Vector3d u = n.unitOrthogonal(), v = u.cross(n);
    Vector3d u = Vector3d::Random().normalized().cross(n), v = u.cross(n);

    Matrix3d rot_objspc_to_wrldspc;
    rot_objspc_to_wrldspc<<u,v,n;
    m = (rot_objspc_to_wrldspc*m).normalized();

    return m;
}

void GGX::eval(Ray& r, const Vector3d& p, const Vector3d& n) {
    Vector3d m = sample_microfacet_normal(n);
    Vector3d o = (-2*r.dir.dot(m)*m+r.dir);

    double weight = r.dir.dot(m)*G(-r.dir, o, m, n)/(r.dir.dot(n)*m.dot(n));
    double chi = (m.dot(-r.dir)>0) ? 1.0 : 0.0;

    r.color = r.color*chi*F(-r.dir, o, m)*G(-r.dir, o, m, n)*m.dot(o)/(n.dot(-r.dir)*m.dot(n));
    r.dir = o;
    r.origin = p+n*0.01;
}


GGX::~GGX() {}
