#ifndef TORUS_H_
#define TORUS_H_

#include "../object.h"
class Torus :  public Object {
public:
    Torus(double const &R, double const &r, Point const &position, Vector const &rotation, Vector const &scale);

    Hit intersect(Ray const &ray) override;
    Point computeUV(const Hit &hit, const Ray &ray) override;

    Point const position;
    Vector const rotation;
    Vector const scale;
    // R: distance to ring "inner radius", r: radius of ring
    double const R;
    double const r;
};


#endif //TORUS_H_
