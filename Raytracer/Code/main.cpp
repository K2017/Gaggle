#include "raytracer.h"

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <iostream>
#include <string>


using namespace std;
namespace mpi = boost::mpi;

int main(int argc, char *argv[])
{
	mpi::environment env;
    cout << "Introduction to Computer Graphics - Raytracer\n\n";

    if (argc < 2 || argc > 3)
    {
        cerr << "Usage: " << argv[0] << "in-file [out-file.png]\n";
        return 1;
    }

    Raytracer raytracer;

    string resources;
    resources = argv[1];
    if(resources.find('/') != string::npos) {
        resources.erase(resources.begin() + resources.find_last_of('/'), resources.end());
        raytracer.resourcePath = resources + "/Resources/";
    }
    
    
    // read the scene
    if (!raytracer.readScene(argv[1]))
    {
        cerr << "Error: reading scene from " << argv[1] <<
            " failed - no output generated.\n";
        return 1;
    }

    // determine output name
    string ofname;
    if (argc >= 3)
    {
        ofname = argv[2];   // use the provided name
    }
    else
    {
        ofname = argv[1];   // replace .json with .png
        ofname.erase(ofname.begin() + ofname.find_last_of('.'), ofname.end());
        ofname += ".png";
    }

    raytracer.renderToFile(ofname);

    return 0;
}
