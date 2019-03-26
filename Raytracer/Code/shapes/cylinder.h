#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "../object.h"

class Cylinder : public Object {
public:
    Cylinder(Point const &top, Point const &bottom, double const &radius);

    Hit intersect(Ray const &ray) override;

    Point const p1, p2;
    double const r;
private:
    double const r2 = r*r;
};


#endif //CYLINDER_H_
