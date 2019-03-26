#include "cone.h"

Cone::Cone(double const &radius1, double const &radius2, Point const &top, const Point &bottom)
:
r1(radius1),
r2(radius2),
p1(bottom),
p2(top)
{}

Hit Cone::intersect(Ray const &ray) {

    Vector O = ray.O;
    Vector D = ray.D;

    double alpha = (r1 - r2) / (p2 - p1).length();

    double cos2alpha = cos(alpha) * cos(alpha);
    double sin2alpha = sin(alpha) * sin(alpha);

    Vector Va = (p2 - p1).normalized();
    Point Pa = p1 + (r1*(p2-p1)/(r1-r2));

    Vector dP = O - Pa;

    double a = cos2alpha*((D - (D.dot(Va))*Va).dot(D - (D.dot(Va))*Va)) - sin2alpha*(D.dot(Va))*(D.dot(Va));
    double b = 2*cos2alpha*((D - (D.dot(Va))*Va).dot(dP - dP.dot(Va)*Va)) - 2*sin2alpha*(D.dot(Va))*(dP.dot(Va));
    double c = cos2alpha*((dP - dP.dot(Va)*Va).dot(dP - dP.dot(Va)*Va)) - sin2alpha*(dP.dot(Va))*(dP.dot(Va));

    double delta = b*b - 4.0*a*c;

    if(delta < 0) {
        return Hit::NO_HIT();
    }

    double t = (-b - sqrt(delta)) / (2.0*a);

    if(t < 0) {
        return Hit::NO_HIT();
    }

    Point P = ray.at(t);

    if((Pa - P).dot(Va) < alpha) {
        // Don't hit the shadowed cone
        return Hit::NO_HIT();
    }

    Vector N = (P - (Pa - (P - Pa).dot(-Va)*Va)).normalized();

    if((P - p2).dot(Va) > 0) {
        //Hit top cap
        N = (Pa - p2).normalized();

        double de = D.dot(N);
        if(fabs(de) < std::numeric_limits<double>::epsilon()) {
            return Hit::NO_HIT();
        }

        t = (N).dot(p2 - O)/de;

        if(t < 0) {
            return Hit::NO_HIT();
        }

        P = ray.at(t);

        Vector v = P - p2;

        if(v.dot(v) > r2*r2) {
            return Hit::NO_HIT();
        }

        return {t, N};
    }

    if((P - p1).dot(Va) < 0) {
        //Hit bottom cap
        N = (p1 - Pa).normalized();

        double de = D.dot(N);
        if(fabs(de) < std::numeric_limits<double>::epsilon()) {
            return Hit::NO_HIT();
        }

        t = (N).dot(p1 - O)/de;

        if(t < 0) {
            return Hit::NO_HIT();
        }

        P = ray.at(t);

        Vector v = P - p1;

        if(v.dot(v) > r1*r1) {
            return Hit::NO_HIT();
        }

        return {t, N};
    }

    return {t, N};
}


