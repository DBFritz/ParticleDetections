#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <array>
#include <string>
#include <cstring>
#include <list>
#include <cmath>
#include "rawImages.hpp"
#include "vec2.hpp"
#include "bitmap_image.hpp"

void raw::event_t::addPixel(unsigned int x, unsigned int y, pixelValue_t value)
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

void raw::event_t::setSaturationValue(pixelValue_t saturation){
    saturationValue = saturation;
}
raw::pixelValue_t raw::event_t::getSaturationValue(){ return saturationValue; }


/// Return the number of saturated Pixels
int raw::event_t::saturatedPixels(){ return saturatedPixels(saturationValue); }
int raw::event_t::saturatedPixels(pixelValue_t saturation)
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

/// Constructor: from a linear stream it cut it and put it in a rawPhoto_t
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
            

void raw::rawPhoto_t::toHistogram(int *toFill)
{
    for(pixelValue_t i=0; i<saturationValue; i++) toFill[(int)std::floor(i)]=0;
    for (unsigned int y=0; y<height; y++)
        for(unsigned int x=0; x<width; x++)
            ++toFill[(int)std::floor(data[y*width+x])];
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

std::list<raw::event_t> raw::rawPhoto_t::findEvents(pixelValue_t trigger) {
    return findEvents(trigger,trigger);
}
std::list<raw::event_t> raw::rawPhoto_t::findEvents(pixelValue_t trigger, pixelValue_t threshold)
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
    return events;
}

//raw::rawPhoto_t raspiraw_to_rawPhoto(const std::string pathFile, const int width, const int height, const int nBadData){
//    return raspiraw_to_rawPhoto(pathFile.c_str(), width, height, nBadData);
//}

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

/*
int main(int argc, char *argv[])
{
    using namespace std;

    char path[64] = "/dev/shm/out.0001.raw";
    const int width=2592, height=1944;
    raw::pixelValue_t *stream = (raw::pixelValue_t*)malloc(height*width*sizeof(raw::pixelValue_t));
    if (stream == NULL){
        cerr << "Error allocating memory" << endl;
        return -1;
    }
    if (raw::raspirawtoArray(stream, path, width, height)) return 0;
    using namespace raw;
    rawPhoto_t rawPhoto(width, height, stream);
    free(stream);

    for(int x=0;x<width;x++)
    {
        double prom =0;
        for(int y=0;y<height;y++)
        {
            prom+=rawPhoto(x,y);
        }
        prom /= height;
        for(int y=0;y<height;y++)
        {
            rawPhoto(x,y)-=prom;
        }
    }
    //rawPhoto.print();

    int toFill[1023] ={0};
    rawPhoto.toHistogram(toFill+50);
    for(int i=0; i<1023;i++)
        cout << i << '\t' << toFill[i] << endl;
}
*/
/*
int main(int argc, char *argv[])
{

    for(int f=1;;f++)
    {
        char path[64];
        sprintf(path, "/dev/shm/out.%04d.raw", f);

        const int width=2592, height=1944;
        raw::pixelValue_t *stream = (raw::pixelValue_t*)malloc(height*width*sizeof(raw::pixelValue_t));
        if (stream == NULL){
            cerr << "Error allocating memory" << endl;
            return -1;
        }

        if (raw::raspirawtoArray(stream, path, width, height)) return 0;

        using namespace raw;
        rawPhoto_t rawPhoto(width, height, stream);
        free(stream);

        cout << rawPhoto(0,0) << endl;
    }
}
*/