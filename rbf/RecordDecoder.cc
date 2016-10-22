///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _recordDecoder_cc_
#define _recordDecoder_cc_


#include <cassert>
#include <string.h>
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


RC RecordDecoder::decodeAttr(byte *dst,
                               const vector<Attribute>& recordDescriptor,
                               const string& attributeName) {
    // Find the position of the attribute
    int attrPos = -1;
    for (int i = 0; i < recordDescriptor.size(); ++i) {
        if (recordDescriptor[i].name == attributeName){
            attrPos = i;
            break;
        }
    }

    if (attrPos == -1) {
        return -1;
    }

    // TODO: restructure and reuse functions
    if (isAttrNull(attrPos)) {
        // Write nulls indicator
        memset(dst, 0x80, 1);
    }
    else {
        // Write nulls indicator
        memset(dst, 0x0, 1);

        // Write data
        byte *attrAddr = getAttrAddr(attrPos);
        memcpy(dst + 1, attrAddr, calcSizeAttrValue(attrPos, attrAddr));
    }

    return 0;
}

unsigned RecordDecoder::calcSizeAttrValue(int n, const byte *itAttr) {
    unsigned size;
    switch (_attrs[n].type) {
        case TypeInt:
            size = _attrs[n].length;
            break;
        case TypeReal:
            size = _attrs[n].length;
            break;
        case TypeVarChar:
            size = ByteArray::decode(itAttr, 4) + 4;
            break;
    }
    return size;
}


byte *RecordDecoder::getAttrAddr(int attrPos) {
    return _data + 2 * attrPos;
}


vector<bool> RecordDecoder::getNullAttrs() {
    LOG("Getting null attributes");

    vector<bool> result(_attrs.size());
    for (int i = 0; i < _attrs.size(); ++i) {
        result[i] = isAttrNull(i);
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


bool RecordDecoder::isAttrNull(int attrPos) {
    unsigned attrOffset = ByteArray::decode(getAttrAddr(attrPos), 2);
    return attrOffset == 0;
}


#undef LOG

#endif
