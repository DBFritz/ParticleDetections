#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <array>
#include <cstring>
#include <list>


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

            // TODO: Cuidado, no verifica si le mandas verdura
            /*
            pixelValue_t& operator[] (int i)
            {
                std::list<monocromePixel_t>::iterator it(pixels.begin());
                std::advance(it, i);
                return *it;
            }*/
            
            /// Push back a new pixel
            void addPixel(unsigned int x, unsigned int y, pixelValue_t value)
            {
                monocromePixel_t newPixel= {x,y,value};
                pixels.push_back(newPixel);
                nPixels++;
            }

            /// Ask if the pixel with coordinates (x,y) is in the Event
            bool isHere(unsigned int x, unsigned int y)
            {
                for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
                    if(it->x == x && it->y == y) return true;
                return false;
            }

            /// Return the size of the event
            int size() { return nPixels; }

            void setSaturationValue(pixelValue_t saturation){
                saturationValue = saturation;
            }
            /// Return the number of saturated Pixels
            int saturatedPixels(pixelValue_t saturation)
            {
                
            }

            /// Return the total charge of the event
            long int charge()
            {
                long int charge =0;
                for (std::list<monocromePixel_t>::iterator it = pixels.begin(); it != pixels.end(); it++)
                    charge += it->value;
                return charge;
            }
    };

    class rawPhoto_t
    {
        public:
            pixelValue_t saturationValue{1023};
            unsigned int width;
            unsigned int height;
            pixelValue_t *data;
        
        public:
            static const int maxValue{1024-1};
            /// Constructor: from a linear stream it cut it and put it in a rawPhoto_t
            rawPhoto_t(unsigned int _width, unsigned int _height, pixelValue_t *stream): width(_width), height(_height)
            {
                data = new pixelValue_t[width*height];
                std::memcpy(data, stream, sizeof(pixelValue_t)*width*height);
            }

            pixelValue_t getValue(unsigned int x, unsigned int y){
                if (0<=x && x<= width  &&  0<=y && y<=height) return  data[y*width+x];
                return 0;
            }
            pixelValue_t& operator() (int x, int y) { return data[y*width+x]; }
            pixelValue_t& operator[] (int i) { return data[i]; }

            void print()                                                    { print(std::cout, width, height, true, '\t'); }
            void print(unsigned int  _width, unsigned int  _height, bool printHeader, char spacer) {print(std::cout,_width,_height,printHeader,spacer); }
            void print(std::ostream output)                                 { print(output, width, height, true, '\t'); }
            void print(std::ostream output, unsigned int  _width, unsigned int  _height)    
                                                                            { print(output, _width, _height, true, '\t'); }
            void print(std::ostream output, bool printHeader)               { print(output, width, height, printHeader, '\t'); }
            void print(std::ostream & output, unsigned int  _width, unsigned int  _height, bool printHeader, char spacer)
            {
                if (printHeader)
                    output << _height << 'x' << _width << std::endl;
                for (unsigned int y=0; y < _height; y++) {
                    for (unsigned int x=0; x<_width; x++)
                        output << data[y*width+x] << spacer;
                    output << std::endl;
                }
            }
            
            static bool isAdjacent(int x_1, int y_1, int x_2, int y_2){
                return ((x_1-1==x_1 || x_1==x_2 || x_1+1==x_2) && (y_1-1==y_2 || y_1==y_2 || y_1+1==y_2));
            }
            
            int recursiveAddingto(event_t * event, int x, int y, pixelValue_t threshold = maxValue/2)
            {
                if (getValue(x,y)<threshold) return 0;
                event->addPixel(x,y, getValue(x,y));
                std::cout << '\t' << "Estoy agregando x=" << x << " y=" << y << std::endl;
                data[y*width+x] = 0; //Apago el pixel para que no lo vuelva a contar
                return 1+recursiveAddingto(event, x+1, y, threshold)+
                        recursiveAddingto(event, x, y+1, threshold)+
                        recursiveAddingto(event, x-1, y, threshold)+
                        recursiveAddingto(event, x, y-1, threshold);
            }

            std::list<event_t> findEvents(pixelValue_t threshold = maxValue/2)
            {
                std::list<event_t> events;
                for (unsigned int y=0; y<height; y++)
                    for(unsigned int x=0; x<width; x++)
                        if ( (*this)(x,y)>threshold)
                        {
                            /*  Es mejor apagar los pixeles y luego prenderlos con su respectivo valor.
                            bool toAdd = true;
                            std::cout << "Encontre un valor grande " << std::endl;
                            for (std::list<event_t>::iterator it = events.begin(); it != events.end(); it++)  // para todos los eventos encontrados
                                if ( (*it).isHere(x,y) )
                                    toAdd =false;*/
                            std::cout << "Encontré un evento!" << std::endl;
                            event_t event;
                            recursiveAddingto(&event,x,y,threshold);
                            events.push_back(event);
                        }
                for (std::list<event_t>::iterator itEvent = events.begin(); itEvent != events.end(); itEvent++)  // para todos los eventos encontrados
                    for (std::list<monocromePixel_t>::iterator itPix = itEvent->pixels.begin(); itPix != itEvent->pixels.end(); itPix++)
                        data[itPix->y * width + itPix->x] = itPix->value;
                return events;
            }
            
    };

    class rawVideo{
        int n;
        rawPhoto_t * frames;
        double dt;
        rawVideo();

    };

    int rawtoArray(pixelValue_t * array, const char * pathFile, const int width = 2592, const int height = 1944, const int nBadData = 24)
    {
        std::ifstream input(pathFile);
        if (!input.good()){
            std::cerr << "Couldn't open " << pathFile << std::endl;
            return -2;
        }
        std::cerr << pathFile << std::endl;

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
}

int main(int argc, char *argv[])
{
    using namespace std;
    
    const int width=2592, height=1944;
    raw::pixelValue_t *stream = (raw::pixelValue_t*)malloc(height*width*sizeof(raw::pixelValue_t));
    if (stream == NULL){
        cerr << "Error allocating memory" << endl;
        return -1;
    }
    for(int y=0;y<height;y++)
        for(int x=0;x<width;x++)
            stream[y*width+x] = 0;
    for(int y=5;y<5+3;y++)
        for(int x=3;x<3+3;x++)
            stream[y*width+x] = 1000;
    stream[6*width+2] = 1000;
    for(int y=4;y<4+4;y++)
        for(int x=10;x<10+2;x++)
            stream[y*width+x] = 720;
    raw::rawPhoto_t photo(width, height,stream);
    free(stream);

    list<raw::event_t> events = photo.findEvents();
    for (list<raw::event_t>::iterator it = events.begin(); it != events.end(); it++)    // Esta es la forma de iterar sobre todos 
        cout << "Carga: " << it->charge() << endl;                                      // los eventos encontrados en una foto. FEA!
    photo.print(20, 15, false, '\t');
    
    /*
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

        if (raw::rawtoArray(stream, path, width, height)) return 0;

        using namespace raw;
        rawPhoto_t rawPhoto(width, height, stream);
        free(stream);

        cout << rawPhoto(0,0) << endl;
    }
    */
}