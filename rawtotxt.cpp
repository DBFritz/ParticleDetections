#include <iostream>
#include <string>
#include "rawImages.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace raw;

    const int width=2592, height=1944;
    rawPhoto_t photo(width,height);

    switch (argc)
    {
        case 1:
            for(int f=1; ;f++)      // the first two images are bullsh**
            {
                char pathRaw[64];
                char pathTxt[64];
                sprintf(pathRaw, "/dev/shm/out.%04d.raw", f);
                cout << "Intentando con el archivo " << pathRaw << '\r' << flush;
                cout << "                                                              \r";
                sprintf(pathTxt, "/dev/shm/out.%04d.txt", f);

                photo.raspiraw(pathRaw);
                if (photo.isEmpty()) return 0;
                if (!photo.isValid()) cerr << "Hay un pixel mas grande de lo que deberia" << endl;

                ofstream output(pathTxt);
                if ( !output.is_open() ) {
                    cerr << "Error al abrir el archivo " << pathTxt << endl;
                    return -1;
                }
                photo.print(output);
            }

        case 2:
            cout << "Usage: " << argv[0] << " /pathto/source /pathto/destination" << endl;

        case 3:

            photo.raspiraw(argv[1]);
            if ( photo.isEmpty() ) {
                cerr << "Error al abrir el archivo " << argv[1] << endl;
            }

            if (!photo.isValid()) cerr << "Hay un pixel mas grande de lo que deberia" << endl;

            ofstream output(argv[2]);
            if ( !output.is_open() ) {
                cerr << "Error al escribir en el archivo " << argv[2] << endl;
                return -1;
            }
            photo.print(output);
    }
    return 0;
}

