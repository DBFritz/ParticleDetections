#include <iostream>
#include <string>
#include "rawImages.hpp"
#include "rawFilters.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace raw;

    const int width=2592, height=1944;
    rawPhoto_t photo(width,height);
    rawPhoto_t master(width,height);

    char the_path[64]="/dev/shm";
    int start = 1, end = 0;

    switch (argc)
    {
        case 4:
            end = strtol(argv[3],NULL,10)+1;

        case 3:
            start = strtol(argv[2],NULL,10);

        case 2:
            strcpy(the_path, argv[1]);

        case 1:
            if (argc>0 && argv[1][strlen(argv[1])-1]=='/')
                argv[1][strlen(argv[1])-1]='\0';
            int f;
            for(f=start; f!=end ;f++)      // the first two images are bullsh**
            {
                char pathRaw[64];
                sprintf(pathRaw, "%s/out.%04d.raw", the_path,f);
                cerr << "Intentando con el archivo " << pathRaw << '\r';

                photo.raspiraw(pathRaw);
                if (photo.isEmpty()) {
                    f--;
                    break;
                }
                master += photo;
            }
            master /= (f-start);
            master.print();
            break;
    }
    return 0;
}

