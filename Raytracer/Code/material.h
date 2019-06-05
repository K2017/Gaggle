#pragma once

#include <utility>
#include <limits>
#include "triple.h"
#include "image.h"

class Material;
typedef std::shared_ptr<Material> MaterialPtr;

class Material {
public:
    Color color;        // base color
    double ka;          // ambient intensity
    double kd;          // diffuse intensity
    double ks;          // specular intensity
    double eta;         // index of refraction
    double scatter;     // refractive scattering
    double n;           // exponent for specular highlight size

    bool refractive;
    bool frosted;

    std::shared_ptr<Image> diffuse;
    std::shared_ptr<Image> specular;
    std::shared_ptr<Image> normal;

    Material() = default;

    Material(Color const &color, double ka, double kd, double ks, double n)
            :
            color(color),
            ka(ka),
            kd(kd),
            ks(ks),
            //eta(2),           // ior of crystal
            eta(1.46),      // ior of plastic
            scatter(0),
            n(n),
            refractive(false),
            frosted(false),
            diffuse(nullptr),
            specular(nullptr),
            normal(nullptr)
            {}
};
