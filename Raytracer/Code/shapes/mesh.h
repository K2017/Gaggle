#ifndef MESH_H_
#define MESH_H_

#include "../object.h"
#include "triangle.h"
#include "../objloader.h"

class Mesh : public Object {
public:
    Mesh(std::string const &filename, Point const &position, Vector const &rotation, Vector const &scale);

    Hit intersect(Ray const &ray) override;
    void applyMaterial(Material const &material) override;

    std::string const modelPath;
    Point const position;
    Vector const rotation;
    Vector const scale;

protected:
    OBJLoader modelLoader;
    std::vector<Triangle> triangles;
    std::vector<Vertex> vertices;
};


#endif //MESH_H_
