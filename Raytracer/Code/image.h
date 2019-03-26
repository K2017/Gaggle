#ifndef IMAGE_H_
#define IMAGE_H_

#include "triple.h"

#include <boost/mpi.hpp>
#include <string>
#include <vector>
#include <memory>


namespace mpi = boost::mpi;

class Image
{
    std::vector<Color> d_pixels;
    std::unique_ptr<Color[]> data;
    unsigned d_width;
    unsigned d_height;
    unsigned blockSize;
    unsigned offset;

    public:
        Image(unsigned width = 0, unsigned height = 0);
        Image(std::string const &filename);

        // normal accessors
        void put_pixel(unsigned x, unsigned y, Color const &c);
        Color get_pixel(unsigned x, unsigned y) const;

        // Handier accessors
        // Usage: color = img(x,y);
        //        img(x,y) = color;
        //Color const &operator()(unsigned x, unsigned y) const;
        //Color &operator()(unsigned x, unsigned y);
        
        Color const &operator()(unsigned i, unsigned j) const {return data[d_width * i + j - offset];}
        Color &operator()(unsigned i, unsigned j) {return data[d_width * i + j - offset];}

        unsigned width() const;
        unsigned height() const;
        unsigned size() const;

        // Normalized accessors, unsignederval is (0...1, 0...1)
        // usefull for texture access
        Color const &colorAt(float x, float y) const;

		void gather();
        void write_png(std::string const &filename) const;
        void read_png(std::string const &filename);
        
        template <typename Function> void
		  each(Function function)
		  {
			for (unsigned i = 0; i < d_height; ++i) {
			  for (unsigned j = 0; j < d_width; ++j) {
				function((*this)(i, j), i, j);
			  }
			}
		  }
		  
		  template <typename Function> void
		  each(Function function) const
		  {
			for (unsigned i = 0; i < d_height; ++i) {
			  for (unsigned j = 0; j < d_width; ++j) {
				function((*this)(i, j), i, j);
			  }
			}
		}

    private:
        inline unsigned index(unsigned x, unsigned y) const
        {
            return y * d_width + x;
        }

        inline unsigned findex(float x, float y) const
        {
            return index(
                static_cast<unsigned>(x * (d_width - 1)),
                static_cast<unsigned>(y * (d_height - 1)));
        }

};

#endif
