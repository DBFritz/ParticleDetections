#include <iostream>
#include "rawImages.hpp"

int main(int argc, char *argv[])
{
    using namespace std;

    const int width=2592, height=1944;

    raw::rawPhoto_t photo(width, height);

    for(int y=5;y<5+3;y++)
        for(int x=3;x<3+3;x++)
            photo(x,y) = 1000;
    photo(6,2) = 600;
    for(int y=4;y<4+4;y++)
        for(int x=10;x<10+2;x++)
            photo(x,y) = 720;
    

    list<raw::event_t> events = photo.findEvents();
    for (list<raw::event_t>::iterator it = events.begin(); it != events.end(); it++)    // Esta es la forma de iterar sobre todos los eventos encontrados en una foto. FEA!
    {
        cout << "Centro en: " << it->center<double>() << "\tancho:" << it->center_sigma() << "\tCarga: " << it->charge() << "\tPixeles Saturados: " << it->saturatedPixels(1000) << endl;
    }
    photo.crop(0,0,20,15).toBitMap_grayscale("output.bmp");

    cout << photo.crop(0,0,14,10);
    return 0;
}