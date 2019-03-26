#include "triangle.h"
#include <cmath>
#include <cfloat>

using namespace std;

Triangle::Triangle(Point const &a, Point const &b, Point const &c)
:
    a(a),
    b(b),
    c(c),
    N()
{
    Vector U(b - a);
    Vector V(c - a);
    N = U.cross(V);
    N.normalize();
}

Hit Triangle::intersect(Ray const &ray) {

    Vector AB = b - a;
    Vector AC = c - a;
    Vector P = ray.D.cross(AC);
    double delta = AB.dot(P);

    // Case The First: triangle is parallel to ray direction
    if(fabs(delta) < DBL_EPSILON) return Hit::NO_HIT();

    double invdelta = 1.0 / delta;

    Vector T = ray.O - a;
    double u = T.dot(P) * invdelta;
    if(u < 0.0 || u > 1.0) return Hit::NO_HIT();

    Vector Q = T.cross(AB);
    double v = ray.D.dot(Q) * invdelta;
    if(v < 0.0 || u + v > 1.0) return Hit::NO_HIT();

    double t = invdelta * AC.dot(Q);

    if(t <= DBL_EPSILON) {
        return Hit::NO_HIT();
    }

    Vector n = N;
    if(N.dot(ray.D) > 0) {
        n = -n;
    }

    return {t, n, material};
}

Point const Triangle::barycenter() {
    return a/3 + b/3 + c/3;
}
