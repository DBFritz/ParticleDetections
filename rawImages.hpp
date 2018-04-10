#ifndef RAW_IMAGES_HPP
#define RAW_IMAGES_HPP

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <array>
#include <cstring>
#include <list>
#include <cmath>
#include "vec2.hpp"
#include "bitmap_image.hpp"

namespace raw{
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

            pixelValue_t getSaturationValue();

            /// Ask if the pixel with coordinates (x,y) is in the Event
            bool isHere(unsigned int x, unsigned int y);

            /// Return the size of the event
            int size();

            void setSaturationValue(pixelValue_t saturation);
            pixelValue_t getSaturationValue(pixelValue_t);

            template <typename R = unsigned short >
            std::vec2<R> center()
            {
                long int w = 0;
                std::vec2<R> pos_center(0,0);
                for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); w += it->value, it++)
                    pos_center.set(it->x*it->value + pos_center.x, it->y * it->value + pos_center.y);
                pos_center /= w;
                return pos_center;
            }

            /// Return the number of saturated Pixels
            int saturatedPixels();
            int saturatedPixels(pixelValue_t saturation);

            /// Return the total charge of the event
            long int charge();
    };

    class rawPhoto_t
    {
        private:
            pixelValue_t saturationValue{1023};
            unsigned int width;
            unsigned int height;
            pixelValue_t *data;
            static bool isAdjacent(int x_1, int y_1, int x_2, int y_2);

            int recursiveAddingto(event_t * event, int x, int y, pixelValue_t threshold = maxValue/2);
        
        public:
            static const int maxValue{1024-1};
            /// Constructor: from a linear stream it cut it and put it in a rawPhoto_t
            rawPhoto_t(unsigned int _width, unsigned int _height, pixelValue_t *stream);
            ~rawPhoto_t();

            rawPhoto_t crop(const unsigned int x_origin,  const unsigned int y_origin, const unsigned int width, const unsigned int height);
            
            pixelValue_t getValue(unsigned int x, unsigned int y);

            // TODO: NO VALIDA
            pixelValue_t& operator() (int x, int y);
            pixelValue_t& operator[] (int i);

            rawPhoto_t& operator=(const rawPhoto_t& photo);

            unsigned int getWidth();
            unsigned int getHeight();
            /// TODO: Poner bien esto, es un pedo.
            /// Instead of printing, I could overlad de << operator. Much better!
            std::ostream& print(std::ostream& output = std::cout, bool printHeader = false, char spacer = '\t');

            friend std::ostream& operator<<(std::ostream &os, raw::rawPhoto_t photo);

            void toBitMap(const std::string& path);
            void toBitMap_grayscale(const std::string& path);
            void toBitMap_grayscale(const std::string& path, pixelValue_t maxValue);

            void toHistogram(int *toFill);
            std::list<event_t> findEvents(pixelValue_t threshold = maxValue/2);


            // ¿Cómo distingo entre canales?        ///

            // TODO: Implement
            pixelValue_t median();
            pixelValue_t mean();

            pixelValue_t getSigmaNoice();   // or double (?) or template (?)
            void substractNoice(double nSigmas);
            void gain(double);

            rawPhoto_t transpose();
            rawPhoto_t operator= (rawPhoto_t toCopy);
            bool operator== (rawPhoto_t toCompare);
            void operator+= (pixelValue_t toAdd);
            void operator-= (pixelValue_t toAdd);

            void toPng(char * path);

    };

    int rawtoArray(pixelValue_t * array, const char * pathFile, const int width = 2592, const int height = 1944, const int nBadData = 24);

    inline std::ostream& operator<<(std::ostream &os, raw::rawPhoto_t photo)
    {
        for (unsigned int y=0; y < photo.height; y++) {
            for (unsigned int x=0; x<photo.width; x++)
                os << photo.data[y*photo.width+x] << '\t';
            os << std::endl;
        }
        return os;
    }
}

#endif