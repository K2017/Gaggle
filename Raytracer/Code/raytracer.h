#pragma once

#include "scene.h"

#include <string>

// Forward declerations
class Light;
class Material;

#include "json/json_fwd.h"

class Raytracer
{

	Scene scene;

    public:

        bool readScene(std::string const &ifname);
        void renderToFile(std::string const &ofname);
        std::string resourcePath;
        
        

    private:

        bool parseObjectNode(nlohmann::json const &node);

        Light parseLightNode(nlohmann::json const &node) const;
        Material parseMaterialNode(nlohmann::json const &node) const;

        unsigned int imgSize[2] = {400, 400};
};
