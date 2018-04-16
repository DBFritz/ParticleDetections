////
////
////


#include <iostream>
#include "rawImages.hpp"
#include <string>

using namespace raw;
using namespace std;

const int width=2592, height=1944;
pixelValue_t trigger=1036, threshold= 50;

int main (int argc, char * argv[])
{
    if (argc>0 && argv[1][strlen(argv[1])-1]=='/') argv[1][strlen(argv[1])-1]='\0'; //elimino el '/' final
    std::vector<unsigned long int> histogram;

    
    rawPhoto_t photo(width, height);

    for(int f=3; f<4 ;f++)
    {
        char pathFile[64];
        sprintf(pathFile, "%s/out.%04d.raw", argv[1], f);
        

        photo.raspiraw(pathFile);
        if (photo.isEmpty()) cerr << "El archivo está vacio";
        cout << photo(width-1, height-1);
        //for (int x=0; x < width; x++)
        //    for (y=0; y < height; y++)
        //        if (photo(x,y) > trigger) cout << x << ' ' << y << endl;
        //event_vec_t events = photo.findEvents(trigger, threshold);

        //for (unsigned int i=0; i < events.size(); i++)
        //{
        //    // long charge = events[i].charge();    
        //    // if ( (unsigned long) charge > histogram.size() ) histogram.resize(charge+1);
        //    // ++histogram[charge];
        //    cout << events[i].pixels.begin()->x << '\t' << events[i].pixels.begin()->y << '\t' << events[i].pixels.begin()->value << endl;
        //}
    }
    //for (unsigned int i=0; i < histogram.size(); i++)
    //cout << i << histogram[i] << endl;
    //return 0;
}