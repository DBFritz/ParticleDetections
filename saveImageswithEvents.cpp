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
    const pixelValue_t threshold = 100;

    for(int f=3; ;f++)      // the first two images are bullsh**
    {
        char pathRaw[64];
        if (argc>0 && argv[1][strlen(argv[1]-1)]=='/') argv[1][strlen(argv[1]-1)]=='0';
        sprintf(pathRaw, "%s/out.%04d.raw", (argc>0 ? argv[1]:"/dev/shm"), f);
        cout << "Intentando con el archivo " << pathRaw << '\r';

        pixelValue_t *stream = (raw::pixelValue_t*)malloc(height*width*sizeof(raw::pixelValue_t));
        if (stream == NULL){
            cerr << "Error allocating memory" << endl;
            return -1;
        }

        if (raw::rawtoArray(stream, pathRaw, width, height)) return 0;

        rawPhoto_t photo(width, height, stream);
        free(stream);

        list<event_t> events = photo.findEvents(threshold);
        if (!events.empty())
        {
            char the_path[64] = "\0";
            time_t now = time(NULL);
            if (now != -1)
                strftime(the_path, 64, "./data/%c.txt", gmtime(&now));
            cout << the_path;
            ofstream output(the_path);
            photo.print(output, false, '\t');
            output.close();
            for (list<event_t>::iterator it = events.begin(); it != events.end(); it++)
            {
                cout << "x = " << it->pixels.begin()->x << "\ty = " << it->pixels.begin()->y << "\tcharge = " << it->charge() << endl;
            }

        }
    }

    return 0;
}

