#include <iostream>
#include <string>
#include "rawImages.hpp"
#include "rawFilters.hpp"

int main(int argc, char *argv[])
{

    using namespace std;
    using namespace raw;

    if (argc <= 2) {
        cout << "usage: " << argv[0] << " /path/to/photo.txt /path/to/new/photo.bmp [minValue] [maxValue]";
        return -1;
    }

    rawPhoto_t photo(argv[1]);

    pixelValue_t minimum = photo.minimum();
    pixelValue_t maximum = photo.maximum();
    if (argc>=4) minimum = strtol(argv[3],NULL,10);
    else cout << "Minimum: " << minimum;
    if (argc>=5) maximum = strtol(argv[4],NULL,10);
    else cout << "\tMaximum: " << maximum;
    photo.toBitMap_grayscale(argv[2],minimum,maximum);
    //photo.toBitMap_grayscale(argv[2]);
    return 0;
}

