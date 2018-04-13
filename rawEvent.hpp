#ifndef RAW_EVENT_HPP
#define RAW_EVENT_HPP

# include <iostream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <cstring>
#include <list>
#include <cmath>
#include "vec2.hpp"
#include "bitmap_image.hpp"
#include "rawEvent.hpp"
#include "rawImages.hpp"

namespace raw {
    typedef unsigned short int pixelValue_t;

    struct monocromePixel_t {
        unsigned int x;
        unsigned int y;
        pixelValue_t value;
    };

    class event_t
    {
        private:
            int nPixels =0;
            pixelValue_t saturationValue{1023};
        
        public:

            std::list<monocromePixel_t> pixels;         // TODO: make it private
            
            /// Push back a new pixel
            void addPixel(unsigned int x, unsigned int y, pixelValue_t value);

            /// Ask if the pixel with coordinates (x,y) is in the Event
            bool isHere(unsigned int x, unsigned int y);

            /// Return the number of pixels of the event
            unsigned int size();
            
            // Return the standard deviation of the event. i.e. the spread of the event
            // It uses the WEIGHTED STANDARD DEVIATION of all points in a event.
            // more information in: https://www.itl.nist.gov/div898/software/dataplot/refman2/ch2/weightsd.pdf
            double center_sigma();

            template <typename R = short int >
            std::vec2<R> center()
            {
                long int w = 0;
                unsigned long int x = 0;
                unsigned long int y = 0;
                for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
                {
                    x += it->x * it->value;
                    y += it->y * it->value;
                    w += it->value;
                }
                std::vec2<R> pos_center(x/w,y/w);
                return pos_center;
            }

            void setSaturationValue(pixelValue_t saturation) { saturationValue = saturation; }
            pixelValue_t getSaturationValue() { return saturationValue; }

            /// Return the number of saturated Pixels
            int saturatedPixels();
            int saturatedPixels(pixelValue_t saturation);

            /// Return the total charge of the event
            long int charge();
    };

    typedef std::vector<event_t> event_vec_t;

}

void raw::event_t::addPixel(unsigned int x, unsigned int y, raw::pixelValue_t value)
{
    monocromePixel_t newPixel= {x,y,value};
    pixels.push_back(newPixel);
    nPixels++;
}


/// Ask if the pixel with coordinates (x,y) is in the Event
bool raw::event_t::isHere(unsigned int x, unsigned int y)
{
    for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
        if(it->x == x && it->y == y) return true;
    return false;
}

unsigned int raw::event_t::size() { return nPixels; }

double raw::event_t::center_sigma()
{
    std::vec2<double> med = center<double>();
    double variance = 0;
    double totalWeight = 0;
    for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
    {
        std::vec2<double> xi(it->x, it->y);
        std::vec2<double> diff = xi-med;
        variance += std::vec2<double>::dot(diff,diff) * it->value;
        totalWeight+= it->value;
    }
    variance /= (nPixels-1)*totalWeight/nPixels;
    return sqrt(variance);
}

/// Return the number of saturated Pixels
int raw::event_t::saturatedPixels(){ return saturatedPixels(saturationValue); }
int raw::event_t::saturatedPixels(raw::pixelValue_t saturation)
{
    int n = 0;
    for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
        if (it->value >= saturation)
            n++;
    return n;
}

/// Return the total charge of the event
long int raw::event_t::charge()
{
    long int charge =0;
    for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
        charge += it->value;
    return charge;
}

#endif