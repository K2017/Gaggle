#include "image.h"

#include "lode/lodepng.h"

#include <iostream>
#include <fstream>

using namespace std;

Image::Image(unsigned width, unsigned height)
:
    d_pixels(width * height),
    d_width(width),
    d_height(height)
{
	blockSize = width * height / mpi::communicator().size();
	offset = blockSize * mpi::communicator().rank();
	data = std::unique_ptr<Color[]>(new Color[blockSize]);
}

Image::Image(string const &filename)
{
    read_png(filename);
}

// normal accessors
void Image::put_pixel(unsigned x, unsigned y, Color const &c)
{
    (*this)(x, y) = c;
}
Color Image::get_pixel(unsigned x, unsigned y) const
{
    return (*this)(x, y);
}

// Handier accessors
// Usage: color = img(x,y);
//        img(x,y) = color;
/*
Color const&Image::operator()(unsigned x, unsigned y) const
{
    return d_pixels.at(index(x, y));
}
Color &Image::operator()(unsigned x, unsigned y)
{
    return d_pixels.at(index(x, y));
}*/

unsigned Image::width() const
{
    return d_width;
}

unsigned Image::height() const
{
    return d_height;
}

unsigned Image::size() const
{
    return d_width * d_height;
}

// Normalized accessors, unsignederval is (0...1, 0...1)
// usefull for texture access
Color const &Image::colorAt(float x, float y) const
{
    return d_pixels.at(findex(x, y));
}

void Image::gather() 
{
	std::unique_ptr<Color[]> newData;
	if(mpi::communicator().rank() == 0) {
		newData = std::unique_ptr<Color[]>(new Color[d_width * d_height]);
	}
	
	mpi::gather(mpi::communicator(), data.get(), blockSize, newData.get(), 0);
	data = std::move(newData);
}

void Image::write_png(std::string const &filename) const
{
	if(mpi::communicator().rank() != 0) {
		return;
	}
	
    vector<unsigned char> image;
    image.reserve(size() * 4);  // reserves size (less allocations)
    /*
    for (Color pixel : d_pixels)
    {
        image.push_back(static_cast<unsigned char>(pixel.r * 255.0));
        image.push_back(static_cast<unsigned char>(pixel.g * 255.0));
        image.push_back(static_cast<unsigned char>(pixel.b * 255.0));
        image.push_back(255);   // alpha is always 1
    }*/
    
   each([&](const Color &pixel, int i, int j) {
		image.push_back(static_cast<unsigned char>(pixel.r * 255.0)); 
		image.push_back(static_cast<unsigned char>(pixel.g * 255.0)); 
		image.push_back(static_cast<unsigned char>(pixel.b * 255.0)); 
		image.push_back(255.0); 
	});
	
    unsigned error = lodepng::encode(filename, image, d_width, d_height);
    if(error) {
		std::cerr << "lodepng encoding error: " << lodepng_error_text(error) << std::endl;
	}
}

void Image::read_png(std::string const &filename)
{
    vector<unsigned char> image;
    lodepng::decode(image, d_width, d_height, filename);
    d_pixels.reserve(size());

    auto imgIter = image.begin();
    while (imgIter != image.end())
    {
        double r = (*imgIter) / 255.0;
        ++imgIter;
        double g = (*imgIter) / 255.0;
        ++imgIter;
        double b = (*imgIter) / 255.0;
        ++imgIter;
        // Ignore Alpha
        ++imgIter;
        d_pixels.push_back(Color(r, g, b));
    }
}