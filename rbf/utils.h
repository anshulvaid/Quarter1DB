#ifndef __utils_h_
#define __utils_h_

#include <iostream>

#if DEBUG
    #define __LOG__(user, msg) \
            std::cerr << "[" << user << "] " << msg << std::endl
#else
    #define __LOG__(user, msg) ;
#endif


using namespace std;


class ByteArray {
public:
    static unsigned char *encode(unsigned value) {
        for (int i = 0; i < sizeof(value); ++i) {
            _auxByteArray[i] = value & 0xFF;
            value = value >> 8;
        }
        return _auxByteArray;
    }

    static unsigned decode(const byte *arr, size_t n) {
        unsigned res = 0;
        for (int i = 0; i < n; ++i) {
            res |= (arr[i] << i*8);
        }
        return res;
    }

    // Auxiliar variable to store value of an unsigned as a byte array.
    // This we avoid creating this temporary object every time we need one.
    static unsigned char _auxByteArray[sizeof(unsigned)];
};

unsigned char ByteArray::_auxByteArray[sizeof(unsigned)];



#endif