#pragma once

#include "material.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "triple.h"
#include <cml/cml.h>

#include <memory>
class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object {
public:
    Material material;

    virtual ~Object() = default;

    Hit tryHit(Ray const &ray);

    void setMaterial(Material const &m);

    void setPosition(Point const &position);
    void setRotation(Vector const &rotation);
    void setScale(Vector const &scale);

    virtual Point computeUV(const Hit &hit, const Ray &ray);

protected:
    virtual Hit intersect(Ray const &ray) = 0;  // must be implemented
    virtual void applyMaterial(Material const &m) {};  // may be overridden for more complex shapes e.g. multi-material

    cml::matrix44d objectToWorld = cml::matrix44d().identity();
    cml::matrix44d worldToObject = cml::matrix44d().identity();
    cml::matrix44d normTransform = cml::matrix44d().identity();

    cml::matrix44d tTranslation = cml::matrix44d().identity();
    cml::matrix44d tRotation = cml::matrix44d().identity();
    cml::matrix44d tScale = cml::matrix44d().identity();

private:

    void applyTransform();
};
