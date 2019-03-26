#include "raytracer.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/torus.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "shapes/mesh.h"
#include "shapes/mesh_spikey.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parameters ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];
        obj = ObjectPtr(new Sphere(pos, radius));
        if(node.find("rotation") != node.end()) {
            Vector rot(node["rotation"]);
            obj.get()->setRotation(rot);
        }
    }
    else if (node["type"] == "triangle")
    {
        Point a(node["point_a"]);
        Point b(node["point_b"]);
        Point c(node["point_c"]);
        obj = ObjectPtr(new Triangle(a, b, c));
    }
    else if (node["type"] == "torus")
    {
        Point pos(node["position"]);
        Vector rot(node["rotation"]);
        Vector scl(node["scale"]);
        double R = node["inner radius"];
        double r = node["ring radius"];
        obj = ObjectPtr(new Torus(R, r, pos, rot, scl));
    }
    else if (node["type"] == "cone") {
        double r1 = node["bottom radius"];
        double r2 = node["top radius"];
        Point p1(node["top"]);
        Point p2(node["bottom"]);
        obj = ObjectPtr(new Cone(r1, r2, p1, p2));
    }
    else if (node["type"] == "cylinder") {
        Point top(node["top"]);
        Point bottom(node["bottom"]);
        double r = node["radius"];
        obj = ObjectPtr(new Cylinder(top, bottom, r));
    }
    else if (node["type"] == "mesh") {
        std::string path = node["model"];
        path = resourcePath + path;
        Point pos(node["position"]);
        Vector rot(node["rotation"]);
        Vector scl(node["scale"]);
        obj = ObjectPtr(new Mesh(path, pos, rot, scl));
    }
    else if (node["type"] == "spikeymesh") {
        std::string path = node["model"];
        path = resourcePath + path;
        Point pos(node["position"]);
        Vector rot(node["rotation"]);
        Vector scl(node["scale"]);
        Material spikemat = parseMaterialNode(node["spikematerial"]);
        obj = ObjectPtr(new SpikeyMesh(path, pos, rot, scl, spikemat));
    }
    else
    {
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->setMaterial(parseMaterialNode(node["material"]));
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return {pos, col};
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    Material material(Color(), ka, kd, ks, n);

    if(node.find("diffuse") != node.end()) {
        std::string path = node["diffuse"];
        path = resourcePath + path;
        std::cerr << path << std::endl;
        material.diffuse = std::make_shared<Image>(Image(path));
    }
    if(node.find("specular") != node.end()) {
        std::string path = node["specular"];
        path = resourcePath + path;
        std::cerr << path << std::endl;
        material.specular = std::make_shared<Image>(Image(path));
    }
    if(node.find("normal") != node.end()) {
        std::string path = node["normal"];
        path = resourcePath + path;
        std::cerr << path << std::endl;
        material.normal = std::make_shared<Image>(Image(path));
    }
    if(node.find("color") != node.end()) {
        Color color(node["color"]);
        material.color = color;
    }
    if(node.find("ior") != node.end()) {
        double eta = node["ior"];
        material.eta = eta;
    }
    if(node.find("refract") != node.end()) {
        bool refract = node["refract"];
        material.refractive = refract;
    }
    if(node.find("scatter") != node.end()) {
        double scatter = node["scatter"];
        material.scatter = scatter;
        if(scatter > 0) {
            material.frosted = true;
        }
    }

    return material;
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    if(jsonscene.find("Look") != jsonscene.end()) {
	Vector look(jsonscene["Look"]);
	scene.setLook(look);
    } else {
    	scene.setLook(Vector(0,0,0));
    }

    // TODO: add your other configuration settings here

    if(jsonscene.find("Shadows") != jsonscene.end()) {
      scene.shadows = jsonscene["Shadows"];
    } else {
      scene.shadows = false;
    }

    if(jsonscene.find("SoftShadows") != jsonscene.end()) {
      scene.softShadows = jsonscene["SoftShadows"];
    } else {
      scene.softShadows = false;
    }

    if(jsonscene.find("Reflections") != jsonscene.end()) {
        scene.reflection = jsonscene["Reflections"];
    } else {
        scene.reflection = false;
    }

    if(jsonscene.find("Refraction") != jsonscene.end()) {
        scene.refraction = jsonscene["Refraction"];
    } else {
        scene.refraction = false;
    }

    if(jsonscene.find("ReflectionScattering") != jsonscene.end()) {
        scene.reflectionScattering = jsonscene["ReflectionScattering"];
    } else {
        scene.reflectionScattering = false;
    }

    if(jsonscene.find("RefractionScattering") != jsonscene.end()) {
        scene.refractionScattering = jsonscene["RefractionScattering"];
    } else {
        scene.refractionScattering = false;
    }

    if(jsonscene.find("SuperSamplingFactor") != jsonscene.end()) {
      scene.superSamplingFactor = jsonscene["SuperSamplingFactor"];
    } else {
      scene.superSamplingFactor = 1;
    }

    if(jsonscene.find("MaxRecursionDepth") != jsonscene.end()) {
        scene.maxDepth = jsonscene["MaxRecursionDepth"];
    } else {
        scene.maxDepth = 0;
    }


    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
