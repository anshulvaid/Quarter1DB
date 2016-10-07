///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _recordEncoder_cc_
#define _recordEncoder_cc_



RecordEncoder::RecordEncoder(const char *data, const vector<Attribute>& attrs)
: _data(data), _attrs(attrs) {
    calcAttrsSizes(_data + calcNullsIndicatorSize());

}

void RecordEncoder::encode(char *dst) const {
    dst += encodeHeader(dst);
    dst += encodeBody(dst);
}

// Returns the number of bytes written
unsigned RecordEncoder::encodeHeader(char *dst) const {
    unsigned nAttrs = _attrs.size();
    unsigned offset = 2 * nAttrs;
    int i;
    for (i = 0; i < nAttrs; ++i) {
        memcpy(dst + 2 * i, ByteArray::encode( isNull(i) ? 0 : offset), 2);
        offset += _attrsSizes[0];
    }
    return 2 * i;
}

// Returns the number of bytes written
unsigned RecordEncoder::encodeBody(char *dst) const {
    memcpy(dst, _data + calcNullsIndicatorSize(), _totalAttrsSizes);
    return _totalAttrsSizes;
}

unsigned RecordEncoder::sizeAfterEncode() const {
    return 2 + _attrs.size() + _totalAttrsSizes;
}

bool RecordEncoder::isNull(int n) const {
    int byteOffset = n / 8;
    return (_data[byteOffset] & (1 << (8 - (n % 8) - 1))) != 0;
}

unsigned RecordEncoder::calcNullsIndicatorSize() const {
    return ceil((double) _attrs.size() / 8);
}

void RecordEncoder::calcAttrsSizes(const char *attrsData) {
    _attrsSizes = vector<unsigned>(_attrs.size());
    const char *it = attrsData;
    int i = 0; _totalAttrsSizes = 0;
    for(auto& s: _attrsSizes) {
        s = calcSizeAttrValue(i++, it);
        it += s;
        _totalAttrsSizes += s;
    }
}

unsigned RecordEncoder::calcSizeAttrValue(int n, const char *itAttr) {
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


#endif
