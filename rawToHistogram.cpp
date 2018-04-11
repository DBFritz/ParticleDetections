#include <iostream>
#include <string>
#include "rawImages.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace raw;

    const int width=2592, height=1944;
    rawPhoto_t photo(width,height);

    char filePath[64];
    strcpy(filePath, (argc>0 ? argv[1] : "/dev/shm/out.0003.raw") );

    photo.raspiraw(filePath);
    if (photo.isEmpty()) return -1;

    histogram_t histogram = photo.toHistogram();

    for (unsigned int i=0; i<histogram.size(); i++)
        cout << i << '\t' << histogram[i] << endl;
    
    return 0;
}

