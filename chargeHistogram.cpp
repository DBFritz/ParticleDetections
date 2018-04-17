////
////
////


#include <iostream>
#include "rawImages.hpp"
#include <string>

using namespace raw;
using namespace std;

const int width=2592, height=1944;
pixelValue_t trigger=150, threshold= 50;

int main (int argc, char * argv[])
{
    if (argc==0) cerr << "Usage: " << argv[0] << "/folder/to/files [trigger] [threshold]";
    if (argc>0 && argv[1][strlen(argv[1])-1]=='/') argv[1][strlen(argv[1])-1]='\0'; //elimino el '/' final
    std::vector<unsigned long int> histogram;

    if (argc>3){
        trigger = stoi(argv[2]);
        threshold = stoi(argv[3]);
    }
    
    rawPhoto_t photo(width, height);

    for(int f=3; ;f++)
    {
        char pathFile[64];
        sprintf(pathFile, "%s/out.%04d.raw", argv[1], f);
        cerr << "\rIntentando con el archivo " << pathFile; 

        photo.raspiraw(pathFile);
        if (photo.isEmpty()) break;

        // Resto el valor de cada columna a cada columna
        for(int x=0; x < width; x++) {
            double mean = 0;
            for(int y=0; y < height; y++)
                if (photo.getValue(x,y) > threshold)
                    mean += photo.getValue(x,y);
            mean /= height;
            for(int y=0; y < height; y++)
                photo(x,y) -= (photo.getValue(x,y) > mean ? mean : 0);
        }

        event_vec_t events = photo.findEvents(trigger, threshold);

        for (unsigned int i=0; i < events.size(); i++)
        {
            long charge = events[i].charge();    
            if ( (unsigned long) charge > histogram.size() ) histogram.resize(charge+1);
                ++histogram[charge];
        }
    }
    cerr << endl;
    for (unsigned int i=0; i < histogram.size(); i++)
        cout << i << '\t' << histogram[i] << endl;
    return 0;
}