#ifndef RAW_FILTERS_HPP
#define RAW_FILTERS_HPP

#include "rawImages.hpp"

namespace raw{

    ///
    int substract_mean_per_column(raw::rawPhoto_t &photo, raw::pixelValue_t top = 0)
    {
        for(unsigned int x=0; x < photo.getWidth(); x++) {
            double mean = 0;
            for(unsigned int y=0; y < photo.getHeight(); y++)
                if (top == 0 || photo.getValue(x,y) < top)
                    mean += photo.getValue(x,y);
            mean /= photo.getHeight();
            for(unsigned int y=0; y < photo.getHeight(); y++) {
                if ( photo.getValue(x,y) > mean )
                    photo(x,y) -= mean;
                else
                    photo(x,y) = 0;
            }
        }
        return 0;
    }
}

#endif