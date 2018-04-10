#include <iostream>
#include <time.h>
#include <string>
#include "rawImages.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace raw;

    const int width=2592, height=1944;
    const pixelValue_t trigger = 90, threshold = 75;

    rawPhoto_t photo(width,height);
    for(int f=3; ;f++)      // the first two images are bullsh**
    {
        char pathRaw[64];
        sprintf(pathRaw, "/home/fritz/PiSHM/out.%04d.raw", f);
        cout << "Intentando con el archivo " << pathRaw << '\r' << flush;
        cout << "                                                              \r";

        photo.raspiraw(pathRaw);
        if ( photo.isEmpty() ) break;

        list<event_t> events = photo.findEvents(trigger, threshold);
        if (!events.empty())
        {
            char the_path[128] = "\0";
            time_t now = time(NULL);
            if (now != -1)
                strftime(the_path, 128, "./data/%F_%H-%M-%S.%Z.txt", gmtime(&now));

            ofstream output(the_path);
            if ( !output.is_open() ) {
                cerr << "Error al abrir el archivo " << the_path << endl;
                return -1;
            }
            cout << "Escribiendo en:    " << the_path << "     " << std::endl;

            photo.print(output, false, '\t');
            output.close();
            for (list<event_t>::iterator it = events.begin(); it != events.end(); it++)
            {
                cout << "Center: " << it->center() 
                    << "\tSigma: " << it->center_sigma()
                    << "\tTamaño:" << it->size()
                    << "\tCharge = " << it->charge() << endl;
                strftime(the_path, 128, "./results/%F_%H-%M-%S.%Z.bmp", gmtime(&now));
                photo.crop(it->center<double>(), 60, 60).toBitMap_grayscale(the_path);
            }

        }
    }

    return 0;
}

