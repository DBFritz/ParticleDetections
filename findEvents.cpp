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
    cerr << "El formato del archivo es:" << endl;
    cerr << "Centro\t" << "Sigma_centro\t" << "Area\t" << "Carga\t" << "Pixeles Saturados\t" << "Archivo\t" << "LISTA_X_Y_VALUE" << endl;
    for(int f=1; ;f++)
    {
        char pathFile[64];
        sprintf(pathFile, "%s/out.%04d.raw", argv[1], f);
        cerr << "\rIntentando con el archivo " << pathFile; 

        photo.raspiraw(pathFile);
        if (photo.isEmpty() ) break;
        if (photo.mostFreqValue() > threshold){
            cerr << "\rLa foto " << pathFile << " está mala" << endl;
            continue;
        }

        // Resto el valor medio de cada columna a cada columna
        substract_mean_per_column(photo);

        event_vec_t events = photo.findEvents(trigger, threshold);

        for (unsigned int i=0; i < events.size(); i++)
        {
            cout << events[i].center() << '\t'
                << events[i].center_sigma() << '\t'
                << events[i].size() << '\t'
                << events[i].charge() << '\t'
                << events[i].saturatedPixels() << '\t'
                << f << '\t';
            //cout << "Pixels:\t";
            //for (std::list<monocromePixel_t>::iterator it = events[i].pixels.begin(); it != events[i].pixels.end(); it++)
            //    cout << it->x << '\t' << it->y << '\t' << it->value << '\t';
            cout << endl;
        }
    }
    cerr << endl;
    for (unsigned int i=0; i < histogram.size(); i++)
        cout << i << '\t' << histogram[i] << endl;
    return 0;
}