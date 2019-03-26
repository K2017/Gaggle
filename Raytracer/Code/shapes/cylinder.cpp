#include "cylinder.h"

Cylinder::Cylinder(Point const &top, Point const &bottom, double const &radius)
:
p1(top),
p2(bottom),
r(radius)
{}

Hit Cylinder::intersect(Ray const &ray) {

    Vector O = ray.O;
    Vector D = ray.D;

    Point Pa = p2;
    Vector Va = (p1 - p2).normalized();

    Vector dP = O - Pa;

    double a = (D - D.dot(Va)*Va).dot(D - D.dot(Va)*Va);
    double b = 2*((D - D.dot(Va)*Va).dot(dP - dP.dot(Va)*Va));
    double c = ((dP - dP.dot(Va)*Va).dot(dP - dP.dot(Va)*Va)) - r*r;

    double delta = b*b - 4*a*c;

    if(delta < 0) {
        return Hit::NO_HIT();
    }

    double t = (-b - sqrt(delta)) / (2.0*a);

    if(t < 0) {
        return Hit::NO_HIT();
    }

    Point P = O + D*t;

    Vector N = (P - (p2 + (P - p2).dot(Va)*Va)).normalized();

    if((P - p1).dot(Va) > 0) {
        //Hit top cap
        N = (p1 - p2).normalized();;

        double de = D.dot(N);
        if(de == 0) {
            return Hit::NO_HIT();
        }

        t = (N).dot(p1 - O)/de;

        if(t < 0) {
            return Hit::NO_HIT();
        }

        P = ray.at(t);

        Vector v = P - p1;

        if(v.dot(v) > r*r) {
            return Hit::NO_HIT();
        }

        return {t, N};
    }

    if((P - p2).dot(Va) < 0) {
        //Hit bottom cap
        N = (p2 - p1).normalized();

        double de = D.dot(N);
        if(de == 0) {
            return Hit::NO_HIT();
        }

        t = (N).dot(p2 - O)/de;

        if(t < 0) {
            return Hit::NO_HIT();
        }

        P = ray.at(t);

        Vector v = P - p2;

        if(v.dot(v) > r*r) {
            return Hit::NO_HIT();
        }

        return {t, N};
    }

    return {t, N};
}
