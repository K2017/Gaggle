#ifndef CONE_H_
#define CONE_H_

#include "../object.h"

class Cone : public Object {
public:
    Cone(double const &radius1, double const &radius2, Point const &top, Point const &bottom);

    Hit intersect(Ray const &ray) override;

    double const r1, r2;
    Point const p1, p2;
};


#endif //CONE_H_
