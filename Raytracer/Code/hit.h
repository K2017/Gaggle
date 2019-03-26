#include <memory>

#ifndef HIT_H_
#define HIT_H_

#include "triple.h"
#include <limits>

class Hit {
public:
    double t;   // distance of hit
    Vector N;   // Normal at hit
    MaterialPtr Mat; // Material at hit

    Hit(double time, Vector const &normal, Material &material)
            :
            t(time),
            N(normal),
            Mat(std::make_shared<Material>(material)) {}

    Hit(double time, Vector const &normal)
            :
            t(time),
            N(normal),
            Mat(nullptr) {}


    static Hit const NO_HIT() {
        static Hit no_hit(std::numeric_limits<double>::quiet_NaN(),
                          Vector(std::numeric_limits<double>::quiet_NaN(),
                                 std::numeric_limits<double>::quiet_NaN(),
                                 std::numeric_limits<double>::quiet_NaN()));
        return no_hit;
    }
};

#endif
