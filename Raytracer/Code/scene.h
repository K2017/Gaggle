#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <vector>
#include <random>

namespace mpi = boost::mpi;
// Forward declerations
class Ray;
class Image;

class Scene
{
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    Point eye;
    Vector look;    
    

public:
	
    // trace a ray into the scene and return the color
    Color trace(Ray const &ray, int depth);

    // render the scene to the given image
    void render(Image &img);


    void addObject(ObjectPtr obj);
    void addLight(Light const &light);
    void setEye(Triple const &position);
    void setLook(Triple const &rotation);
    unsigned getNumObject();
    unsigned getNumLights();

    Color background = Color(0.5, 0.5, 0.7);
    bool shadows = false;
    bool softShadows = false;
    bool reflection = false;
    bool reflectionScattering = false;
    bool refraction = false;
    bool refractionScattering = false;

    int minSmoothReflectionSamples = 5;
    int minScatteringSamples = 5;
    int maxDepth = 0;
    int superSamplingFactor = 1;

private:
    std::mt19937 generator;

    double randomDouble(const double &min, const double &max);

    Vector const reflect(const Vector &I, const Vector &N);
    Vector const refract(const Vector &I, const Vector &N, const double &ior);
    double fresnel(const Vector &I, const Vector &N, const double &ior);
};

#endif
