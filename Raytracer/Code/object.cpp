#include "object.h"

Hit Object::tryHit(Ray const &ray) {
    // transform ray to object space
    cml::vector4d rayDir = worldToObject * cml::vector4d(ray.D.x, ray.D.y, ray.D.z, 0);
    cml::vector4d rayOrigin = worldToObject * cml::vector4d(ray.O.x, ray.O.y, ray.O.z, 1);

    Ray transformedRay = Ray(Point(rayOrigin[0], rayOrigin[1], rayOrigin[2]), Vector(rayDir[0], rayDir[1], rayDir[2]).normalized());

    // perform intersection with transformed ray
    Hit out = intersect(transformedRay);

    // find hit point in object space
    Point P = transformedRay.at(out.t);

    // transform hit point back to world space
    auto worldPoint = objectToWorld * cml::vector4d(P.x, P.y, P.z, 1);
    double newT = (ray.O - Vector(worldPoint[0], worldPoint[1], worldPoint[2])).length();

    // transform normals
    cml::vector4d normal = normTransform * cml::vector4d(out.N.x, out.N.y, out.N.z, 0);

    out.N = Vector(normal[0], normal[1], normal[2]).normalized();
    out.t = newT;

    return out;
}

void Object::setMaterial(Material const &m) {
    material = m;
    applyMaterial(m);
}

void Object::setPosition(Point const &position) {
    cml::matrix_translation(tTranslation, cml::vector3d(position.x, position.y, position.z));
    applyTransform();
}
void Object::setRotation(Vector const &rotation) {
    cml::matrix_rotation_euler(tRotation, cml::vector3d(rotation.x*M_PI/180.0, rotation.y*M_PI/180.0, rotation.z*M_PI/180.0), cml::euler_order_xyz);
    applyTransform();
}
void Object::setScale(Vector const &scale) {
    cml::matrix_scale(tScale, cml::vector3d(scale.x, scale.y, scale.z));
    applyTransform();
}

void Object::applyTransform() {
    objectToWorld = tTranslation * tRotation * tScale;

    worldToObject = objectToWorld;
    worldToObject.inverse();

    normTransform = worldToObject;
    normTransform.transpose();
}

Point Object::computeUV(Hit const &hit, const Ray &ray) {
    return Point();
}
