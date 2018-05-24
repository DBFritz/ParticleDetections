#ifndef RAW_FILTERS_HPP
#define RAW_FILTERS_HPP

#include "rawImages.hpp"

namespace raw{

    ///FIXME: toma en cuentra TODOS los valores para el promedio, no solo los valores que ha sumado
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

    ///
    int substract_mean_per_color(raw::rawPhoto_t &photo, raw::pixelValue_t top = 0)
    {
        double mean = 0;
        for(int group=0;group<4;group++) {
            for(unsigned int x=group%2; x < photo.getWidth(); x+=2)
                for(unsigned int y=group/2; y < photo.getHeight(); y+=2)
                    if (top == 0 || photo.getValue(x,y) < top)
                        mean += photo.getValue(x,y);
            for(unsigned int x=group%2; x < photo.getWidth(); x+=2)
                for(unsigned int y=group/2; y < photo.getHeight(); y+=2)
                    if ( photo.getValue(x,y) > mean )
                        photo(x,y) -= mean;
                    else
                        photo(x,y) = 0;
        }
        return 0;
    }
}

#endif