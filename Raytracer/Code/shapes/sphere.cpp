#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    Vector OC = (ray.O - position);

    double a = ray.D.dot(ray.D);
    double b = 2.0 * OC.dot(ray.D);
    double c = OC.dot(OC) - r*r;

    double delta = b*b - 4*a*c;

    if (delta < 0) {
        return Hit::NO_HIT();
    }

    double t0 = (-b - sqrt(delta)) / (2.0*a);
    double t1 = (-b + sqrt(delta)) / (2.0*a);

    if(t0 < 0) {
        t0 = t1;
        if(t0 < 0) {
            return Hit::NO_HIT();
        }
    }

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    Vector N = (ray.at(t0) - position).normalized(); // Normal = Point of intersection - Center
    if(N.dot(ray.D) > 0) {
        N = -N;
    }
    return Hit(t0,N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    r(radius)
{
    setPosition(pos);
}

Point Sphere::computeUV(Hit const &hit, const Ray &ray) {
    Point p = ray.at(hit.t);
    auto c = worldToObject * cml::vector4d(p.x,p.y,p.z,1);

    p = Point(c[0], c[1], c[2]);
    double theta = acos((p.z - position.z)/r);
    double phi = -atan2(p.y - position.y, p.x - position.x);

    double u = phi / (2.0*M_PI);
    double v = 1.0 - (theta / M_PI);

    Point out(u, v, 0);
    return out;
}
