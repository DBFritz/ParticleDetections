#include <iostream>
#include <time.h>
#include <string>
#include "rawImages.hpp"

/// TODO: Comentar funcionamiento
int main(int argc, char *argv[])
{
    using namespace std;
    using namespace raw;

    const int width=2592, height=1944;
    const pixelValue_t threshold = 55;

    for(int f=3; ;f++)      // the first two images are bullsh**
    {
        char pathRaw[64];
        if (argc>0 && argv[1][strlen(argv[1])-1]=='/') argv[1][strlen(argv[1])-1]='\0';
        sprintf(pathRaw, "%s/out.%04d.raw", (argc>0 ? argv[1]:"/dev/shm"), f);
        cout << "Intentando con el archivo " << pathRaw << '\r';

        rawPhoto_t photo = raspiraw_to_rawPhoto(pathRaw, width, height);

        list<event_t> events = photo.findEvents(threshold);
        if (!events.empty())
        {
            char the_path[64] = "\0";
            time_t now = time(NULL);
            if (now != -1)
                strftime(the_path, 64, "./data/test/%Y-%m-%d_%.txt", gmtime(&now));
            cout << the_path;
            ofstream output(the_path);
            output << photo;
            output.close();
            for (list<event_t>::iterator it = events.begin(); it != events.end(); it++)
            {
                cout << "Center: " << it->center() << "\tCharge = " << it->charge() << endl;
            }

        }
    }

    return 0;
}

