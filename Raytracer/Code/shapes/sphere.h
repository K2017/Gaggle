#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere: public Object {
public:
    Sphere(Point const &pos, double radius);

    Hit intersect(Ray const &ray) override;
    Point computeUV(Hit const &hit, const Ray &ray) override;

    Point const position;
    double const r;
};

#endif
