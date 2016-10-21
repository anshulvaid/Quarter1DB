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

    static int decodeToInt(const byte *arr, size_t n) {
        int res = 0;
        for (int i = 0; i < n; ++i) {
            res |= (arr[i] << i*8);
        }
        return res;
    }

    static float decodeToFloat(const byte *arr, size_t n) {
        float res = 0;
        memcpy(&res, arr, n);
        return res;
    }

    // Auxiliar variable to store value of an unsigned as a byte array.
    // This we avoid creating this temporary object every time we need one.
    static unsigned char _auxByteArray[sizeof(unsigned)];
};


unsigned char ByteArray::_auxByteArray[sizeof(unsigned)];


template<typename T>
struct Maybe {
public:
    Maybe() : _valid(false) {}
    Maybe(T t) : _data(t), _valid(true) {}
    explicit operator bool() const {
        return _valid;
    }
    T operator*() const {
        return _data;
    }
private:
  T _data;
  bool _valid;
};



#endif