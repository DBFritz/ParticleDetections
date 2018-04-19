#include <iostream>
#include <string>
#include "rawImages.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace raw;

    const int width=2592, height=1944;
    rawPhoto_t photo(width,height);
    if (argc ==1 )
    {
        cerr << "Usage: " << argv[0] << " /path/to/source/* or /path/to/file.raw" << endl;
        return -1;
    }
    
    double mean=0;
    double sigma=0;
    double mostFreqValue = 0;
    switch (argv[1][strlen(argv[1])-1])
    {
        case '*':
            argv[1][strlen(argv[1])-1]= '\0';
        case '/':
            char path[64];
            int f;
            for(f=3; ;f++)
            {
                sprintf(path,"%sout.%04d.raw",argv[1], f);

                photo.raspiraw(path);
                if ( photo.isEmpty() ) break;
                cerr << "Intentando con  " << path << '\r';

                sigma += photo.sigma_neg();
                mean += photo.mean();
                mostFreqValue += photo.mostFreqValue();
            }
            cerr << endl;
            cout << "Mean:\t" << mean/f << endl
                 << "Sigma:\t" << sigma/f << endl
                 << "MostFreqValue:\t" << mostFreqValue/f << endl;
                
            break;

        default:
            /// TODO: Implement a single file
            cerr << "Todavía no está lista" << endl;
            break;
    }
    return 0;
}

