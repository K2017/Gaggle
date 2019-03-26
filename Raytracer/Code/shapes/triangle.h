#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <vertex.h>
#include "../object.h"

class Triangle : public Object {

public:
    Triangle(Point const &a, Point const &b, Point const &c);

    Hit intersect(Ray const &ray) override;

    Point const a, b, c;
    Vector N; // normal

    Point const barycenter();
};


#endif //TRIANGLE_H_
