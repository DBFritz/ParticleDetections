////
////
////

#include <iostream>
#include <string>
#include "rawImages.hpp"
#include "rawFilters.hpp"

using namespace raw;
using namespace std;

const int width=2592, height=1944;
pixelValue_t trigger=150, threshold= 50, sizeMin = 0;

int main (int argc, char * argv[])
{
    if (argc==0) cerr << "Usage: " << argv[0] << "/folder/to/files [trigger] [threshold]";
    if (argc>0 && argv[1][strlen(argv[1])-1]=='/') argv[1][strlen(argv[1])-1]='\0'; //elimino el '/' final
    std::vector<unsigned long int> histogram;

    if (argc>3){
        trigger = stoi(argv[2]);
        threshold = stoi(argv[3]);
    }
    if (argc>4) sizeMin = stoi(argv[4]);
    
    rawPhoto_t photo(width, height);

    for(int f=3; ;f++)
    {
        char pathFile[64];
        sprintf(pathFile, "%s/out.%04d.raw", argv[1], f);
        cerr << "\rIntentando con el archivo " << pathFile; 

        photo.raspiraw(pathFile);
        if (photo.isEmpty()) break; 

        substract_mean_per_column(photo);

        if (photo.mean() > threshold){
            cerr << "\rLa foto " << pathFile << " está mala                " << endl;
            continue;
        }

        event_vec_t events = photo.findEvents(trigger, threshold);

        for (unsigned int i=0; i < events.size(); i++)
        {
            long charge = events[i].charge();
            if ( events[i].size() >= sizeMin) {
                if ( (unsigned long) charge > histogram.size() )
                    histogram.resize(charge+1);
                ++histogram[charge];
            }
        }
    }
    cerr << endl;
    for (unsigned int i=0; i < histogram.size(); i++)
        cout << i << '\t' << histogram[i] << '\n';
    return 0;
}