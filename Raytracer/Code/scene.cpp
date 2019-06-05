#include <memory>

#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>
#include <iostream>

using namespace std;

Color Scene::trace(Ray const& ray, int depth) {

  // Find hit object and distance
  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  ObjectPtr obj = nullptr;
  for (unsigned idx = 0; idx != objects.size(); ++idx) {
    Hit hit = objects[idx]->tryHit(ray);
    if (hit.t < min_hit.t) {
      min_hit = hit;
      obj = objects[idx];
    }
  }

  // No hit? Return background color.
  if (!obj) return background;

  Material material = obj->material;             //the hit objects material (default)
  if (min_hit.Mat != nullptr) {
    material = *min_hit.Mat.get();             //use material from hit if available
  }

  Point hit = ray.at(min_hit.t);                 //the hit point
  Vector N = min_hit.N;                          //the normal at hit point
  Vector bias = 1e-4 * N;                        //bias along normal
  bool outside = ray.D.dot(N) < 0;
  Vector V = -ray.D;                             //the view vector

  /****************************************************
  * This is where you should insert the color
  * calculation (Phong model).
  *
  * Given: material, hit, N, V, lights[]
  * Sought: color
  *
  * Hints: (see triple.h)
  *        Triple.dot(Vector) dot product
  *        Vector + Vector    vector sum
  *        Vector - Vector    vector difference
  *        Point - Point      yields vector
  *        Vector.normalize() normalizes vector, returns length
  *        double * Color     scales each color component (r,g,b)
  *        Color * Color      dito
  *        pow(a,b)           a to the power of b
  ****************************************************/

  Color color = material.color;

  auto texCoord = obj->computeUV(min_hit, ray);
  auto tX = static_cast<float>(texCoord.x);
  auto tY = static_cast<float>(texCoord.y);
  if (material.diffuse) {
    color = material.diffuse->colorAt(tX, tY);
  }

  if (material.normal) {
    // compute tangent vector
    Vector t = N.cross(Vector(0, 1, 0));
    Vector b;
    if (!t.length()) {
      t = N.cross(Vector(0, 0, 1));
    }
    t.normalize();

    //compute bitangent
    b = N.cross(t).normalized();

    // map normals to [0..1]
    Vector texNorm = (2 * material.normal->colorAt(tX, tY)) - 1;
    texNorm.normalize();

    // create new basis
    auto tbn = cml::matrix33d();
    tbn = tbn.set_col(0, cml::vector3d(t.x, t.y, t.z));
    tbn = tbn.set_col(1, cml::vector3d(b.x, b.y, b.z));
    tbn = tbn.set_col(2, cml::vector3d(N.x, N.y, N.z));

    auto newN = tbn * cml::vector3d(texNorm.x, texNorm.y, texNorm.z);

    N = Vector(newN[0], newN[1], newN[2]).normalized();
  }

  if (material.specular) {
    material.ks *= cml::clamp(material.specular->colorAt(tX, tY).length(), 0.0, 1.0);
  }

  Color I_A, I_D, I_S;

  for (unsigned i = 0; i < getNumLights(); i++) {
    auto& l = lights[i];

    Color lc = l->color;
    Point lp = l->position;

    Vector L = (lp - hit).normalized(); // light vector
    Vector R = reflect(-L, N).normalized(); // reflectance vector
    bias = 1e-4 * L;
    Point shadowPos = outside ? hit + bias : hit - bias;

    if (shadows) {
      bool occluded = false;
      if (softShadows) {
        const cml::vector3d r(L.x, L.y, L.z);
        const cml::vector3d n(N.x, N.y, N.z);
        const cml::vector3d v1 = cml::normalize(cml::cross(r, n));
        const cml::vector3d v2 = cml::normalize(cml::cross(n, v1));
        double coneAngle = 10.0 * M_PI / 180.0;

        int rsSamples = minScatteringSamples;

        for (int s = 0; s < rsSamples; s++) {
          auto result = cml::rotate_vector(r, v1, randomDouble(-1.0, 1.0) * coneAngle);
          auto out = cml::rotate_vector(result, v2, randomDouble(-1.0, 1.0) * coneAngle);
          Vector samp = Vector(out[0], out[1], out[2]).normalized();
          Ray lightRay(shadowPos, samp);
          Hit m_hit = Hit(numeric_limits<double>::infinity(), Vector());
          for (unsigned idx = 0; idx != objects.size(); ++idx) {
            Hit h = objects[idx]->tryHit(lightRay);
            if (h.t < m_hit.t) {
              // hit something
              occluded = true;
              break;
            }
          }

          if (!occluded) {
            I_D += lc * max(0.0, N.dot(L)) * 1.0 / rsSamples * max(0.0, N.dot(samp));
            I_S += lc * pow(max(0.0, R.dot(V)), material.n);
          }
        }
      } else {
        Ray lightRay(shadowPos, L);
        Hit m_hit = Hit(numeric_limits<double>::infinity(), Vector());
        for (unsigned idx = 0; idx != objects.size(); ++idx) {
          Hit h = objects[idx]->tryHit(lightRay);
          if (h.t < m_hit.t) {
            // hit something
            occluded = true;
            break;
          }
        }

        if (!occluded) {
          I_D += lc * max(0.0, N.dot(L));
          I_S += lc * pow(max(0.0, R.dot(V)), material.n);
        }
      }
    } else {
      I_D += lc * max(0.0, N.dot(L));
      I_S += lc * pow(max(0.0, R.dot(V)), material.n);
    }
  }


  // fresnel reflection + refraction
  Color rf(0, 0, 0), rl(0, 0, 0);

  double kr = fresnel(ray.D, N, material.eta);

  if (refraction && material.refractive && kr < 1 && material.ks > 0) {
    Vector rfDir = refract(ray.D, N, material.eta).normalized();
    bias = 1e-4 * rfDir;
    Point rfPos = outside ? hit + bias : hit - bias;
    if (refractionScattering && material.frosted) {
      const cml::vector3d r(rfDir.x, rfDir.y, rfDir.z);
      const cml::vector3d n(N.x, N.y, N.z);
      const cml::vector3d v1 = cml::normalize(cml::cross(r, n));
      const cml::vector3d v2 = cml::normalize(cml::cross(n, v1));
      double coneAngle = 5.0 * material.scatter * M_PI / 180.0;

      int rfSamples = minScatteringSamples + (int) round(10.0 * (1 - material.scatter));

      for (int s = 0; s < rfSamples; s++) {
        auto result = cml::rotate_vector(r, v1, (material.scatter) * randomDouble(-1.0, 1.0) * coneAngle);
        auto out = cml::rotate_vector(result, v2, (material.scatter) * randomDouble(-1.0, 1.0) * coneAngle);
        Vector samp = Vector(out[0], out[1], out[2]).normalized();

        if (depth < maxDepth) {
          rf += trace(Ray(rfPos, samp), depth + 1);
        }
      }
      rf /= rfSamples;
    } else {
      if (depth < maxDepth) {
        rf = trace(Ray(rfPos, rfDir), depth + 1);
      }
    }
  }

  if (reflection && material.ks > 0) {
    Vector rlDir = reflect(ray.D, N).normalized();
    bias = 1e-4 * rlDir;
    Point rlPos = outside ? hit - bias : hit + bias;
    if (reflectionScattering) {
      const cml::vector3d r(rlDir.x, rlDir.y, rlDir.z);
      const cml::vector3d n(N.x, N.y, N.z);
      const cml::vector3d v1 = cml::normalize(cml::cross(r, n));
      const cml::vector3d v2 = cml::normalize(cml::cross(n, v1));
      double coneAngle = (5.0 * (1 - material.ks)) * M_PI / 180.0;

      int rlSamples = minSmoothReflectionSamples + (int) round(10.0 * (material.ks));

      for (int s = 0; s < rlSamples; s++) {
        auto result = cml::rotate_vector(r, v1, (1 - material.ks) * randomDouble(-1.0, 1.0) * coneAngle);
        auto out = cml::rotate_vector(result, v2, (1 - material.ks) * randomDouble(-1.0, 1.0) * coneAngle);
        Vector samp = Vector(out[0], out[1], out[2]).normalized();

        if (depth < maxDepth) {
          rl += trace(Ray(rlPos, samp), depth + 1);
        }
      }
      rl /= rlSamples;
    } else {
      if (depth < maxDepth) {
        rl = trace(Ray(rlPos, rlDir), depth + 1);
      }
    }
  }

  Color rfrl = rl * kr + rf * (1 - kr);
  // end fresnel


  I_A = color * material.ka / lights.size();
  I_D = I_D * color * material.kd;
  I_S = I_S * kr * material.ks;

  return (I_A + I_D + I_S + rfrl);
}

void Scene::render(Image& img) {
  unsigned w = img.width();
  unsigned h = img.height();
  int ss = superSamplingFactor;
  int numSamples = ss * ss;
  cml::matrix44d lookMatrix;
  cml::matrix_rotation_euler(lookMatrix,
                             cml::vector3d(look.x * M_PI / 180.0, look.y * M_PI / 180.0, look.z * M_PI / 180.0),
                             cml::euler_order_xyz);
#pragma acc parallel loop
  for (int y = 0; y < h; ++y) {
    for(int x = 0; x < w; ++x) {
      Color col;
      for (int si = 0; si < ss; si++) {
        for (int sj = 0; sj < ss; sj++) {
          Point pixel(y + ((si + 0.5) / ss), h - 1 - x + ((sj + 0.5) / ss), 0);
          Vector rayDir = (pixel - eye).normalized();
          //auto dir = lookMatrix * cml::vector4d(rayDir.x, rayDir.y, rayDir.z, 0);
          Ray ray(eye, Vector(rayDir.x, rayDir.y, rayDir.z).normalized());
          col += trace(ray, 0);
        }
      }
      col /= numSamples;
      col.clamp();
      img(y, x) = col;
    }
  }
}

// --- Specialised calculations -----------------------------------------------------

Vector const Scene::reflect(const Vector& I, const Vector& N) {
  return I - 2 * I.dot(N) * N;
}

Vector const Scene::refract(const Vector& I, const Vector& N, const double& ior) {
  Vector n = N;
  double cTheta = cml::clamp(I.dot(N), -1.0, 1.0);
  double etai = 1;
  double etat = ior;
  if (cTheta < 0) {
    cTheta *= -1;
  } else {
    std::swap(etai, etat);
    n *= -1;
  }
  double eta = etai / etat;
  double k = 1 - eta * eta * (1 - cTheta * cTheta);
  return k < 0 ? Vector() : eta * I + (eta * cTheta - sqrt(k)) * n;
}

double Scene::fresnel(const Vector& I, const Vector& N, const double& ior) {
  double kr;
  double cTheta = cml::clamp(I.dot(N), -1.0, 1.0);
  double etai = 1;
  double etat = ior;
  if (cTheta > 0) {
    std::swap(etai, etat);
  }
  double sPhi = etai / etat * sqrt(std::max(0.0, 1 - cTheta * cTheta));

  if (sPhi >= 1) {
    kr = 1;
  } else {
    double cPhi = sqrt(std::max(0.0, 1 - sPhi * sPhi));
    cTheta = abs(cTheta);
    double Rs = ((etat * cTheta) - (etai * cPhi)) / ((etat * cTheta) + (etai * cPhi));
    double Rp = ((etai * cTheta) - (etat * cPhi)) / ((etai * cTheta) + (etat * cPhi));
    kr = (Rs * Rs + Rp * Rp) / 2.0;
  }

  return kr;
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj) {
  objects.push_back(obj);
}

void Scene::addLight(Light const& light) {
  lights.push_back(std::make_shared<Light>(light));
}

void Scene::setEye(Triple const& position) {
  eye = position;
}

void Scene::setLook(Triple const& rotation) {
  look = rotation;
}

unsigned Scene::getNumObject() {
  return objects.size();
}

unsigned Scene::getNumLights() {
  return lights.size();
}

double Scene::randomDouble(const double& min, const double& max) {
  uniform_real_distribution<double> distribution(min, max);
  return distribution(generator);
}
