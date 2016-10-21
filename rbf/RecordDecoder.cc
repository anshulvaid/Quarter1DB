///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _recordDecoder_cc_
#define _recordDecoder_cc_


#include <cassert>
#include "utils.h"
#ifndef LOG   // avoid overwriting definition
    #define LOG(msg) __LOG__("RecordDecoder", msg)
#endif


RecordDecoder::RecordDecoder(const vector<Attribute>& recordDescriptor)
        : _data(NULL), _size(0), _attrs(recordDescriptor){
}

RecordDecoder::RecordDecoder(byte *data, unsigned size,
                             const vector<Attribute>& recordDescriptor)
        : _data(data),
          _size(size),
          _attrs(recordDescriptor){

}

void RecordDecoder::decode(byte *dst) {
    assert(_data != NULL);

    LOG("Decoding record to address " << (void *) dst);
    dst += decodeHeader(dst);
    dst += decodeBody(dst);
}

unsigned RecordDecoder::decodeHeader(byte *dst) {
    LOG("Decoding record header to address " << (void *) dst);
    return decodeNullsIndicator(dst, getNullAttrs());
}

vector<bool> RecordDecoder::getNullAttrs() {
    LOG("Getting null attributes");

    vector<bool> result(_attrs.size());
    for (int i = 0; i < _attrs.size(); ++i) {
        unsigned attrOffset = ByteArray::decode(_data + 2 * i, 2);
        result[i] = (attrOffset == 0);
        LOG("Attribute " << i << " at offset " << attrOffset << " is "
            << (!result[i] ? "not " : "") << "null");
    }
    return result;
}

unsigned RecordDecoder::decodeNullsIndicator(byte *dst,
                                        const vector<bool>& nullsIndicator) {
    LOG("Getting nulls indicator");

    byte b, mask;
    byte *writePos = dst;
    for (int i = 0; i < _attrs.size(); ++i) {
        // Reset byte to write and mask
        if (i % 8 == 0) {
            mask = 0x80;
            b = 0;
        }

        // Check if null
        if (nullsIndicator[i]) {
            b |= mask;
        }

        // Advance mask
        mask >>= 1;

        // Write byte
        if (i == _attrs.size() - 1 || mask == 0 ) {
            LOG("Byte " << (i / 8) << " of nulls indicator has value "
                    << hex << (unsigned) b);
            *writePos++ = b;
        }
    }

    LOG("Decoded nulls indicator has size " << writePos - dst << " bytes");
    return writePos - dst;
}


unsigned RecordDecoder::decodeBody(byte *dst) {
    LOG("Decoding record body to address " << (void *) dst);
    unsigned headerSize = 2 * _attrs.size();
    unsigned bodySize = _size - headerSize;
    memcpy(dst, _data + headerSize, bodySize);
    return bodySize;
}


void RecordDecoder::setRecordData(byte *recordAddr, unsigned size) {
    _data = recordAddr;
    _size = size;
}

#undef LOG

#endif
