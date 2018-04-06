#include <iostream>
#include "rawImages.hpp"

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
    stream[6*width+2] = 600;
    for(int y=4;y<4+4;y++)
        for(int x=10;x<10+2;x++)
            stream[y*width+x] = 720;
    raw::rawPhoto_t photo(width, height,stream);
    free(stream);

    list<raw::event_t> events = photo.findEvents();
    for (list<raw::event_t>::iterator it = events.begin(); it != events.end(); it++)    // Esta es la forma de iterar sobre todos los eventos encontrados en una foto. FEA!
    {
        cout << "Center: (" << it->center<double>().x << ',' << it->center<double>().y << ")\tCarga: " << it->charge() << endl;
    }
    photo.print(20, 15, false, '\t');


    return 0;
}