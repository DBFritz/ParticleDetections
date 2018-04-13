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
#include "rawEvent.hpp"

namespace raw{
    typedef unsigned short int pixelValue_t;
    typedef std::vector<unsigned int> histogram_t;

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
            /// Constructors:
            // Photo with all pixels set in 0
            rawPhoto_t(unsigned int _width, unsigned int _height);
            // Photo with the pixels from the stream array
            rawPhoto_t(unsigned int _width, unsigned int _height, pixelValue_t *stream);
            ~rawPhoto_t();

            bool raspiraw(const char * filename, const int nBadData = 24);
            rawPhoto_t crop(const std::vec2<short int> center, int width, int height);
            rawPhoto_t crop(const unsigned int x_origin,  const unsigned int y_origin, unsigned int width, unsigned int height);
            
            pixelValue_t getValue(unsigned int x, unsigned int y);

            // TODO: NO VALIDA
            pixelValue_t& operator() (int x, int y);
            pixelValue_t& operator[] (int i);

            rawPhoto_t& operator=(const rawPhoto_t& photo);

            bool isEmpty();

            unsigned int getWidth();
            unsigned int getHeight();

            void setSaturationValue(pixelValue_t saturation) { saturationValue = saturation; }
            pixelValue_t getSaturationValue() { return saturationValue; }

            /// TODO: Poner bien esto, es un pedo.
            /// Instead of printing, I could overlad de << operator. Much better!
            std::ostream& print(std::ostream& output = std::cout, bool printHeader = false, char spacer = '\t');

            friend std::ostream& operator<<(std::ostream &os, raw::rawPhoto_t photo);

            void toBitMap(const std::string& path);
            void toBitMap_grayscale(const std::string& path);
            void toBitMap_grayscale(const std::string& path, pixelValue_t maxValue);

            std::vector<unsigned int> toHistogram();
            std::vector<event_t> findEvents(pixelValue_t trigger = maxValue/2);
            std::vector<event_t> findEvents(pixelValue_t trigger, pixelValue_t threshold);

            // TODO: Implement
            pixelValue_t median();
            double mean();
            pixelValue_t mostFreqValue();

            double sigma_neg();   // or double (?) or template (?)
            void substractNoice(double nSigmas);
            void gain(double);

            rawPhoto_t transpose();
            rawPhoto_t operator= (rawPhoto_t toCopy);
            bool operator== (rawPhoto_t toCompare);
            void operator+= (pixelValue_t toAdd);
            void operator-= (pixelValue_t toAdd);

    };

    int raspirawtoArray(pixelValue_t * array, const char * pathFile, const int width = 2592, const int height = 1944, const int nBadData = 24);

    //raw::rawPhoto_t raspiraw_to_rawPhoto(const std::string pathFile, const int width = 2592, const int height = 1944, const int nBadData = 24);

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

raw::rawPhoto_t::rawPhoto_t(unsigned int _width, unsigned int _height): width(_width), height(_height)
{
    data = new pixelValue_t[width*height];
    std::fill_n(data, width*height, 0);
}
raw::rawPhoto_t::rawPhoto_t(unsigned int _width, unsigned int _height, pixelValue_t *stream): width(_width), height(_height)
{
    data = new pixelValue_t[width*height];
    std::memcpy(data, stream, sizeof(pixelValue_t)*width*height);
}

raw::rawPhoto_t::~rawPhoto_t(){
    delete[] data;
}

bool raw::rawPhoto_t::raspiraw(const char * filename, const int nBadData)
{
    raw::pixelValue_t *stream = new raw::pixelValue_t[height*width];
    if (raw::raspirawtoArray(stream, filename, width, height, nBadData))
    {
        std::fill_n(data, width*height, 0);
        return false;
    }
    std::memcpy(data, stream, sizeof(pixelValue_t)*width*height);
    delete[] stream;
    return true;
}

raw::rawPhoto_t raw::rawPhoto_t::crop(std::vec2<double> center, double width, double height)
{
    return crop(center.x - width/2, center.y - height/2, width, height);
}
raw::rawPhoto_t raw::rawPhoto_t::crop(const unsigned int x_origin, const unsigned int y_origin, const unsigned int width, const unsigned int height)
{
    raw::pixelValue_t * stream = new pixelValue_t[width*height];
    for(unsigned int y=0; y<height; y++)
        std::memcpy(stream+y*width, data+x_origin+(y_origin+y) * this->width, sizeof(pixelValue_t)*width);
    raw::rawPhoto_t cropped(width, height, stream);
    delete[] stream;
    return cropped;
}

raw::pixelValue_t raw::rawPhoto_t::getValue(unsigned int x, unsigned int y){
    if (0<=x && x<= width  &&  0<=y && y<=height) return  data[y*width+x];
    return 0;
}

// TODO: NO VALIDA que exista
raw::pixelValue_t& raw::rawPhoto_t::operator() (int x, int y) {
    return data[y*width+x]; 
    }
raw::pixelValue_t& raw::rawPhoto_t::operator[] (int i) { return data[i]; }

raw::rawPhoto_t& raw::rawPhoto_t::operator=(const raw::rawPhoto_t& photo)
{
    width = this->width;
    height = this->height;
    data = new raw::pixelValue_t[width*height];
    std::memcpy(data, this->data, sizeof(raw::pixelValue_t)*width*height);
    return *this;
}

bool raw::rawPhoto_t::isEmpty()
{
    for (unsigned int y=0; y < height; y++) 
        for (unsigned int x=0; x<width; x++)
            if (getValue(x,y) != 0) return false;
    return true;
}

unsigned int raw::rawPhoto_t::getWidth() { return width; }
unsigned int raw::rawPhoto_t::getHeight() { return height; }

std::ostream& raw::rawPhoto_t::print(std::ostream& output, bool printHeader, char spacer)
{
    if (printHeader)
        output << height << 'x' << width << std::endl;
    for (unsigned int y=0; y < height; y++) {
        for (unsigned int x=0; x<width; x++)
            output << data[y*width+x] << spacer;
        output << std::endl;
    }
    return output;
}

void raw::rawPhoto_t::toBitMap(const std::string& path)
{
    // TODO: COMPLETAR para exportar la imagen en color.
}

void raw::rawPhoto_t::toBitMap_grayscale(const std::string& path) { toBitMap_grayscale(path, saturationValue); }
void raw::rawPhoto_t::toBitMap_grayscale(const std::string& path, raw::pixelValue_t maxValue)
{
    bitmap::bitmap_image output(width, height);
    for (unsigned int y=0; y<height; y++)
        for (unsigned int x=0; x<width; x++)
            output.set_pixel(x,y, 
                data[y*width+x]*bitmap::MAX_VALUE_PER_PIXEL_PER_COLOR/maxValue,
                data[y*width+x]*bitmap::MAX_VALUE_PER_PIXEL_PER_COLOR/maxValue,
                data[y*width+x]*bitmap::MAX_VALUE_PER_PIXEL_PER_COLOR/maxValue);
    output.save_image(path);
}

bool raw::rawPhoto_t::isAdjacent(int x_1, int y_1, int x_2, int y_2){
    return ((x_1-1==x_1 || x_1==x_2 || x_1+1==x_2) && (y_1-1==y_2 || y_1==y_2 || y_1+1==y_2));
}
            

std::vector<unsigned int> raw::rawPhoto_t::toHistogram()
{
    std::vector<unsigned int> toFill;
    toFill.resize(saturationValue+1);
    //for(pixelValue_t i=0; i<=saturationValue; i++) toFill[(int)std::floor(i)]=0;
    for (unsigned int y=0; y<height; y++)
        for(unsigned int x=0; x<width; x++)
            ++toFill[(int)std::floor(data[y*width+x])];
    return toFill;
}

int raw::rawPhoto_t::recursiveAddingto(event_t * event, int x, int y, pixelValue_t threshold)
{
    if (getValue(x,y)<threshold) return 0;
    event->addPixel(x,y, getValue(x,y));
    data[y*width+x] = 0; //Apago el pixel para que no lo vuelva a contar
    return 1+recursiveAddingto(event, x+1, y, threshold)+   recursiveAddingto(event, x+1, y+1, threshold)+
            recursiveAddingto(event, x, y+1, threshold)+    recursiveAddingto(event, x-1, y+1, threshold)+
            recursiveAddingto(event, x-1, y, threshold)+    recursiveAddingto(event, x-1, y-1, threshold)+
            recursiveAddingto(event, x, y-1, threshold)+    recursiveAddingto(event, x+1, y-1, threshold);
}

std::vector<raw::event_t> raw::rawPhoto_t::findEvents(pixelValue_t trigger) {
    return findEvents(trigger,trigger);
}
std::vector<raw::event_t> raw::rawPhoto_t::findEvents(pixelValue_t trigger, pixelValue_t threshold)
{
    std::list<raw::event_t> events;
    for (unsigned int y=0; y<height; y++)
        for(unsigned int x=0; x<width; x++)
            if ( (*this)(x,y)>trigger)
            {
                raw::event_t event;
                recursiveAddingto(&event,x,y,threshold);
                events.push_back(event);
            }
    for (std::list<raw::event_t>::iterator itEvent = events.begin(); itEvent != events.end(); itEvent++)  // para todos los eventos encontrados
        for (std::list<monocromePixel_t>::iterator itPix = itEvent->pixels.begin(); itPix != itEvent->pixels.end(); itPix++)
            data[itPix->y * width + itPix->x] = itPix->value;
    return std::vector<raw::event_t>( std::begin(events), std::end(events) );
}

double raw::rawPhoto_t::mean()
{
    double sum = 0;
    for (unsigned int y=0; y<height; y++)
        for(unsigned int x=0; x<width; x++)
            sum += getValue(x,y);
    return sum/(width*height);
}

raw::pixelValue_t raw::rawPhoto_t::mostFreqValue()
{
    std::vector<unsigned int> histogram = toHistogram();
    pixelValue_t most=0;
    for (pixelValue_t i = 1; i < saturationValue; i++)
        if ( histogram[i] > histogram[most])
            most = i;
    return most;
}

// Compute the sigma using the values that are less than the mostFrequentValue;
double raw::rawPhoto_t::sigma_neg() {
    std::vector<unsigned int> histogram = toHistogram();
    pixelValue_t most=0;
    for (pixelValue_t i = 1; i < saturationValue; i++)
        if ( histogram[i] > histogram[most])
            most = i;

    double variance = 0;
    long sumHisto = 0;
    int m=0;
    for(pixelValue_t i=most-1; i > 0; i++) {
        variance += (most-i)*(most-i) * histogram[i];
        if (histogram[i] != 0) m++;
        sumHisto += histogram[i];
    }
    return sqrt(m * variance / ( sumHisto * (m-1) ));
}


int raw::raspirawtoArray(pixelValue_t * array, const char * pathFile, const int width, const int height, const int nBadData)
{
    std::ifstream input(pathFile);
    if (!input.good()){
        std::cerr << "Couldn't open " << pathFile << std::endl;
        return -2;
    }

    // Horizontally, each row consists of 10-bit values. Every four bytes are
    // the high 8-bits of four values, and the 5th byte contains the packed low
    // 2-bits of the preceding four values. In other words, the bits of the
    // values A, B, C, D and arranged like so:
    //
    //  byte 1   byte 2   byte 3   byte 4   byte 5
    // AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD AABBCCDD
    // after each row, there are nBadData that shouldn't be read

    char byteread[5];
    int v;
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x+=4) { // read in groups of 4 pixels a full line
            input.read(byteread,5);
            for (int i=0;i<4;i++) {
                v=byteread[i]<<2;
                v+=(byteread[4]>> 2*(3-i))&0x3;
                array[y*width+x+i]=v;
            }
        }
        input.seekg(nBadData,std::ios_base::cur);
    }
    input.close();
    return 0;
}

#endif