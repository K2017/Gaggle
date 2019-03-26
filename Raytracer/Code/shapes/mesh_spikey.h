#ifndef MESH_SPIKEY_H_
#define MESH_SPIKEY_H_

#include "../object.h"
#include "cone.h"
#include "triangle.h"
#include "../objloader.h"

class SpikeyMesh : public Object {
public:
    SpikeyMesh(std::string const &filename, Point const &position, Vector const &rotation, Vector const &scale, Material const &spikematerial);

    Hit intersect(Ray const &ray) override;
    void applyMaterial(Material const &material) override;

    std::string const modelPath;
    Point const position;
    Vector const rotation;
    Vector const scale;


private:
    OBJLoader modelLoader;
    std::vector<Triangle> triangles;
    std::vector<Cone> cones;
};


#endif //MESH_SPIKEY_H_
