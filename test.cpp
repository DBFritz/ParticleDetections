#include <iostream>
#include "rawImages.hpp"

int main(int argc, char *argv[])
{
    using namespace std;

    const int width=2592, height=1944;

    raw::rawPhoto_t photo(width, height);

    for(int y=5;y<5+3;y++)
        for(int x=3;x<3+3;x++)
            photo(x,y) = 512;
    photo(2,6) = 73;
    for(int y=4;y<4+4;y++)
        for(int x=10;x<10+2;x++)
            photo(x,y) = 256;
    
    raw::event_vec_t events = photo.findEvents(250,100);
    for (unsigned int i; i < events.size(); i++) 
    {
        cout << "Centro en: " << events[i].center<double>()
            << "\tancho:" << events[i].center_sigma()
            << "\tCarga: " << events[i].charge()
            << "\tPixeles Saturados: " << events[i].saturatedPixels(1000) << endl;
    }
    photo.crop(events[0].center(), 20, 20).toBitMap_grayscale("./local/test_output.bmp");

    return 0;
}