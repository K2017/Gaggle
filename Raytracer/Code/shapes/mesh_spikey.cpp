#include "mesh_spikey.h"

SpikeyMesh::SpikeyMesh(std::string const &filename, Point const &position, Vector const &rotation, Vector const &scale, Material const &spikematerial)
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

    double avgScale = (scale.x + scale.y + scale.z)/3.0;

    modelLoader.unitize();
    std::vector<Vertex> vertices = modelLoader.vertex_data();
    for(unsigned int i = 0; i < vertices.size(); i += 3) {
        Vertex a = vertices[i];
        Vertex b = vertices[i+1];
        Vertex c = vertices[i+2];

        Triangle t(Point(a.x, a.y, a.z), Point(b.x, b.y, b.z), Point(c.x, c.y, c.z));
        t.material = material;
        triangles.push_back(t);

        Cone cone(1.0/avgScale, 0, t.barycenter() + t.N*(24.0/avgScale), t.barycenter());
        cone.material = spikematerial;
        cones.push_back(cone);
    }
}

Hit SpikeyMesh::intersect(Ray const &ray) {

    Hit min_hit(std::numeric_limits<double>::infinity(), Vector());

    for(unsigned int i = 0; i < triangles.size(); i++) {
        Triangle t = triangles[i];
        Cone c = cones[i];

        Hit hit = t.intersect(ray);
        Hit chit = c.intersect(ray);

        if(hit.t < min_hit.t) {
            min_hit = hit;
        }
        if (chit.t < min_hit.t) {
            min_hit = chit;
        }
    }

    if(min_hit.t == std::numeric_limits<double>::infinity()) {
        return Hit::NO_HIT();
    }

    return min_hit;
}

void SpikeyMesh::applyMaterial(Material const &mat) {
    for(auto &t : triangles) {
        t.material = mat;
    }
}
