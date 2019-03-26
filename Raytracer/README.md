# Raytracer C++ framework for Introduction to Computer Graphics

This is a C++ framework for a raytracer. It is created for the Computer
Science course "Introduction to Computer Graphics" taught at the
University of Groningen.

This framework is inspired by and uses (some) code of the raytracer framework of
Bert Freudenberg which unfortunately is no longer available.

## Compiling the code

We provided a [CMakeLists.txt](CMakeLists.txt) file to be used with `cmake`.
This can be used in the following way (on the command line/shell):
```
# cd into the main directory of the framework
mkdir build   # create a new build directory
cd build      # cd into the directory
cmake ..      # creates a Makefile
# Comipiling the code can then be done with:
make
# or
make -j4      # replacing 4 with the number of cores of your pc
```
**Note!** After adding new `.cpp` files (when adding new shapes)
`cmake ..` needs to be called again or you might get linker errors.

## Running the Raytracer
After compilation you should have the `ray` executable.
This can be used like this:
```
./ray <path to .json file> [output .png file]
# when in the build directory:
./ray ../Scenes/scene01.json
```
Specifying an output is optional and by default an image will be created in
the same directory as the source scene file with the `.json` extension replaced
by `.png`.

## Description of the included files

### Scene files
* `Scenes/*.json`: Scene files are structured in JSON. If you have never
    worked with json, please see [here](https://en.wikipedia.org/wiki/JSON#Data_types,_syntax_and_example)
    or [here](https://www.json.org/).

    Take a look at the provided example scenes for the general structure.
    You are free (and encouraged) to define your own scene files later on.

### The raytracer source files (Code directory)

* `main.cpp`: Contains main(), starting point. Responsible for parsing
    command-line arguments.

* `raytracer.cpp/.h`: Raytracer class. Responsible for reading the scene
    description, starting the raytracer and writing the result to an image file.

* `scene.cpp/.h`: Scene class. Contains code for the actual raytracing.

* `image.cpp/.h`: Image class, includes code for reading from and writing to PNG
    files.

* `light.h`: Light class. Plain Old Data (POD) class. Colored light at a
    position in the scene.

* `ray.h`: Ray class. POD class. Ray from an origin point in a direction.

* `hit.h`: Hit class. POD class. Intersection between an `Ray` and an `Object`.

* `object.h`: virtual `Object` class. Represents an object in the scene.
    All your shapes should derive from this class. See

* `shapes (directory/folder)`: Folder containing all your shapes.

* `sphere.cpp/.h (inside shapes)`: Sphere class, which is a subclass of the
    `Object` class. Represents a sphere in the scene.

* `example.cpp/.h (inside shapes)`: Example shape class. Copy these two files
    and replace/rename **every** instance of `Example` `example.h` or `EXAMPLE`
    with your new shape name.

* `triple.cpp/.h`: Triple class. Represents a 3-dimensional vector which is
    used for colors, points and vectors.
    Includes a number of useful functions and operators, see the comments in
    `triple.h`.
    Classes of `Color`, `Vector`, `Point` are all aliases of `Triple`.

* `objloader.cpp/.h`: Is a similar class to Model used in the OpenGL
    exercises to load .obj model files. It produces a std::vector
    of Vertex structs. See `vertex.h` on how you can retrieve the
    coordinates and other data defined at vertices.

### Supporting source files (Code directory)

* `lode/*`: Code for reading from and writing to PNG files,
    used by the `Image` class.
    lodepng is created by Lode Vandevenne and can be found on
    [github](https://github.com/lvandeve/lodepng).
* `json/*`: Code for parsing JSON documents.
    Created by Niels Lohmann and available under the MIT license on
    [github](https://github.com/nlohmann/json).
    **Recommended:** Especially take a look at their README for more
    info on how to work with json files.
* `cml/*`: Configurable Math Library: Code for performing linear algebra operations.
    Created by github user demianmnave, available on
    [github](https://github.com/demianmnave/CML) here.


### Additions for assignment (2)

* Added ability to render shadows cast from multiple light sources. 

    Shadow rendering can be controlled through the scene file using the 
    "Shadows": `bool` parameter.
    
    See [Scene::trace(Ray)](Code/scene.cpp) lines `119 - 138` for implementation.
    
* Added Fresnel based reflection and refraction calculations. 

    The index of refraction can be controlled on a per-material basis using the 
    "ior": `double` parameter.
    By default the ior for a material is set to 1.49 which approximates shiny plastic.
    
    A material can be told to refract light using the "refract": `bool` parameter. 
    If this is set to false the ior will affect only the reflectivity of the material
    and it will appear opaque. Essentially use this if you want to enable transparency.
    
    Rendering of reflections and refraction can be controlled on a per-scene basis
    using the "Reflections": `bool` and "Refraction": `bool` parameters.
    
    See [Scene::trace(Ray)](Code/scene.cpp) lines `143 - 209` and `249 - 292` for implementation.
   
* Added support for scattered / diffuse reflections. 
    
    The blurriness of a reflection is controlled directly by the specular 
    coefficient of the material, a more specular material will have sharper,
    more mirror-like reflections and vice versa for less specular materials.
    Materials with no specular will not be taken into account in the reflection
    calculation and will fall back to diffuse (as is to be expected).
    
    Rendering of scattered reflections can be controlled on per-scene basis using the 
    "ReflectionScattering": `bool` parameter. 
    (Will have no effect if reflections are not enabled)
    
    **Note!** Enabling this tends to increase render times by a lot, so only enable if you really need it.
    
* Added support for scattered / diffuse refraction.

    Light scattering within a material can be modeled using the "scatter": `double` parameter 
    (0 for no scattering, 1 for complete scattering), and can be used to produce the effect of
    frosted glass in certain situations.
    
     Rendering of scattered refraction can be controlled on per-scene basis using the 
     "RefractionScattering": `bool` parameter. 
     (Will have no effect if refraction is not enabled)
     
     **Note!** Enabling this tends to increase render times by a lot, so only enable if you really need it.
    
* Added super-sample anti-aliasing support. 
    
    The super-sampling factor can be set in the scene file using the 
    "SuperSamplingFactor": `int` parameter.
    
    The chosen implementation allows for arbitrary integer factors to be used which allows for 
    fine tuning of anti-aliasing per scene.
    
* Added rotation support for spheres.

* Added texture mapping support for spheres and torii (toruses? donuts.).
    
    Materials now have the option to include a diffuse, specular or normal map
    (or a combination of them),
    which will define the color, reflectivity and normals of an object.
    
    If a diffuse map is not defined the "color" parameter will be used instead.
    
* Added lines of feedback below. 

    One of the problems we ran into along the way was not considering shadow acne.
    It took a while to figure that out, also because we did not get it clearly enough
    from the lectures. It would be nice if concepts this important were emphasised more.
    Without this, it is not very clear to us what we should focus on most. 
    That aside, this was rather enjoyable overall.