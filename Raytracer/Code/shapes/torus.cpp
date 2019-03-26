#include <cmath>
#include <cfloat>
#include <iostream>
#include "torus.h"
#include "../polynomial.h"

using namespace std;

Torus::Torus(double const &R, double const &r, Point const &position, Vector const &rotation, Vector const &scale)
        :
        position(position),
        rotation(rotation),
        scale(scale),
        R(R),
        r(r) {
    setPosition(position);
    setRotation(rotation);
    setScale(scale);
}

Hit Torus::intersect(Ray const &ray) {

    Vector O = ray.O;
    Vector D = ray.D;
    double dy = D.y;
    double oy = O.y;

    double rayO2 = O.dot(O);
    double rayD2 = D.dot(D);
    double rayDO = D.dot(O);
    double R2 = R * R;
    double r2 = r * r;

    // coefficients of quartic intersection equation
    double a = (rayO2 - R2 - r2) * (rayO2 - R2 - r2) - 4.0 * R2 * (r2 - oy * oy);
    double b = 4.0 * rayDO * (rayO2 - R2 - r2) + 2.0 * 4.0 * R2 * oy * dy;
    double c = 2.0 * rayD2 * (rayO2 - R2 - r2) + 4.0 * rayDO * rayDO + 4.0 * R2 * dy * dy;
    double d = 4.0 * rayD2 * rayDO;
    double e = rayD2 * rayD2;

    double s[4];
    double coeffs[] = {a, b, c, d, e};

    int solutions = SolveQuartic(coeffs, s);

    if (solutions == 0) {
        return Hit::NO_HIT();
    }

    double minT = FLT_MAX;
    for (double t : s) {
        if ((t > 0.0001) && (t < minT)) {
            minT = t;
        }
    }

    if (minT == FLT_MAX) {
        return Hit::NO_HIT();
    }

    Vector P = ray.at(minT);

    double x = P.x;
    double y = P.y;
    double z = P.z;

    double alpha = 2 * R2;
    double gamma = x * x + y * y + z * z + R2 - r2;

    Vector N = Vector(x * (gamma - alpha), y * gamma, z * (gamma - alpha)).normalized();

    return {minT, N};
}

Point Torus::computeUV(const Hit &hit, const Ray &ray) {

    Point p = ray.at(hit.t);
    auto P = worldToObject * cml::vector4d(p.x, p.y, p.z, 1);
    double x = P[0];
    double y = P[1];
    double z = P[2];

    double u = (1.0 - (atan2(z, x) + M_PI) / (2.0 * M_PI));

    double len = sqrt(x * x + z * z);

    x = len - R;
    double v = (atan2(y, x) + M_PI) / (2.0 * M_PI);

    return Point(u, v, 0);
}
