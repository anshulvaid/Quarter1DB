#include <vector>
#include <iostream>
#include <cassert>
#include "../pfm.h"
#include "../utils.h"

using namespace std;

int main(int argc, char const *argv[]) {

    vector<unsigned> toTry {220, 2, 3, 0, 10, 3213, 0xFF};

    for(auto v: toTry) {
        assert(v == ByteArray::decode(ByteArray::encode(v), 2));
        return 0;
    }
    return 0;
}