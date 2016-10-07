///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _recordDecoder_cc_
#define _recordDecoder_cc_


RecordDecoder::RecordDecoder(char *data, unsigned size,
                             const vector<Attribute>& recordDescriptor)
        : _data(data),
          _size(size),
          _attrs(recordDescriptor){

}

void RecordDecoder::decode(char *dst) {
    dst += decodeHeader(dst);
    dst += decodeBody(dst);
}

unsigned RecordDecoder::decodeHeader(char *dst) {
    vector<bool> nullsIndicator(_attrs.size());
    for (int i = 0; i < _attrs.size(); ++i) {
        unsigned attrOffset = ByteArray::decode(_data + 2 * i, 2);
        nullsIndicator[i] = (attrOffset == 0);
    }

    return decodeNullsIndicator(dst, nullsIndicator);
}

unsigned RecordDecoder::decodeNullsIndicator(char *dst,
                                        const vector<bool>& nullsIndicator) {
    byte b, mask;
    char *writePos = dst;
    for (int i = 0; i < _attrs.size(); ++i) {
        if (i % 8 == 0) {
            mask = 0x80;
            b = 0;

            if (i != 0) {
                *writePos++ = b;
            }
        }

        if (nullsIndicator[i]) {
            b |= mask;
        }
        mask >>= 1;
    }

    return writePos - dst;
}


unsigned RecordDecoder::decodeBody(char *dst) {
    unsigned headerSize = 2 * _attrs.size();
    unsigned bodySize = _size - headerSize;
    memcpy(dst, _data + headerSize, bodySize);
    return bodySize;
}

#endif
