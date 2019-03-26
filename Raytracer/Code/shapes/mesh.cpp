#include "mesh.h"

Mesh::Mesh(std::string const &filename, Point const &position, Vector const &rotation, Vector const &scale)
:
modelPath(filename),
position(position),
rotation(rotation),
scale(scale),
modelLoader(filename)
{
    setPosition(position);
    setRotation(rotation);
    setScale(scale);

    modelLoader.unitize();
    vertices = modelLoader.vertex_data();
    for(unsigned int i = 0; i < vertices.size(); i += 3) {
        Vertex a = vertices[i];
        Vertex b = vertices[i+1];
        Vertex c = vertices[i+2];

        Triangle t(Point(a.x, a.y, a.z), Point(b.x, b.y, b.z), Point(c.x, c.y, c.z));
        triangles.push_back(t);
    }
}

Hit Mesh::intersect(Ray const &ray) {

    Hit min_hit(std::numeric_limits<double>::infinity(), Vector());
    for(Triangle &t : triangles) {
        Hit hit = t.intersect(ray);
        if(hit.t < min_hit.t) {
            min_hit = hit;
        }
    }

    if(min_hit.t == std::numeric_limits<double>::infinity()) {
        return Hit::NO_HIT();
    }
    return min_hit;
}

void Mesh::applyMaterial(Material const &mat) {
    for(Triangle &t : triangles) {
        t.material = mat;
    }
}

