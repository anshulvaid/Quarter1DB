///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _recordEncoder_cc_
#define _recordEncoder_cc_

#include "utils.h"
#ifndef LOG   // avoid overwriting definition
    #define LOG(msg) __LOG__("RecordEncoder", msg)
#endif


RecordEncoder::RecordEncoder(const byte *data, const vector<Attribute>& attrs)
: _data(data), _attrs(attrs) {
    calcAttrsSizes(_data + calcNullsIndicatorSize());
}

void RecordEncoder::encode(byte *dst) const {
    LOG("Encoding record to address " << (void *) dst);
    dst += encodeHeader(dst);
    dst += encodeBody(dst);
}

// Returns the number of bytes written
unsigned RecordEncoder::encodeHeader(byte *dst) const {
    LOG("Encoding record header to address " << (void *) dst);

    unsigned nAttrs = _attrs.size();
    unsigned offset = 2 * nAttrs;
    int i;
    for (i = 0; i < nAttrs; ++i) {
        LOG("Encoding attribute " << i << (isNull(i) ? "(NULL)" : "") << " to address "
                << (void *) (dst + 2 * i));
        memcpy(dst + 2 * i, ByteArray::encode( isNull(i) ? 0 : offset), 2);
        LOG("Offset of attribute " << i << " is " << offset);
        offset += _attrsSizes[i];
    }
    return 2 * i;
}

// Returns the number of bytes written
unsigned RecordEncoder::encodeBody(byte *dst) const {
    memcpy(dst, _data + calcNullsIndicatorSize(), _totalAttrsSizes);
    return _totalAttrsSizes;
}

unsigned RecordEncoder::sizeAfterEncode() const {
    return 2 * _attrs.size() + _totalAttrsSizes;
}

bool RecordEncoder::isNull(int n) const {
    int byteOffset = n / 8;
    return (_data[byteOffset] & (1 << (8 - (n % 8) - 1))) != 0;
}

unsigned RecordEncoder::calcNullsIndicatorSize() const {
    return ceil((double) _attrs.size() / 8);
}

void RecordEncoder::calcAttrsSizes(const byte *attrsData) {
    _attrsSizes = vector<unsigned>(_attrs.size());
    const byte *it = attrsData;
    int i = 0; _totalAttrsSizes = 0;
    for(auto& s: _attrsSizes) {
        s = isNull(i) ? 0 : calcSizeAttrValue(i, it);
        i++;
        it += s;
        _totalAttrsSizes += s;
    }
    LOG("The total size of attributes values is " << _totalAttrsSizes);
}

unsigned RecordEncoder::calcSizeAttrValue(int n, const byte *itAttr) {
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

RC RecordEncoder::printRecord() {
    const byte *itAttrs = _data + calcNullsIndicatorSize();

    for (int i = 0; i < _attrs.size(); ++i) {
        cout << _attrs[i].name.c_str() << ": " << flush;
        if (isNull(i)) {
            printf("NULL\t");
            continue;
        }


        unsigned attrSize = _attrsSizes[i];
        switch (_attrs[i].type) {
            case TypeInt:
                cout << ByteArray::decodeToInt(itAttrs, attrSize) << "\t";
                break;
            case TypeReal:
                cout << (float) ByteArray::decodeToFloat(itAttrs, attrSize)
                     << "\t";
                break;
            case TypeVarChar:
                printf("%.*s\t", attrSize - 4, itAttrs + 4);
                break;
        }
        itAttrs += attrSize;
    }
    cout << endl;
    return 0;
}

#undef LOG

#endif
