#ifndef _recordEncoder_h_
#define _recordEncoder_h_

class RecordEncoder {
public:
    RecordEncoder(const byte *data, const vector<Attribute>& attrs);

    void encode(byte *dst) const;

    // Returns the number of bytes written
    unsigned encodeHeader(byte *dst) const;

    // Returns the number of bytes written
    unsigned encodeBody(byte *dst) const;

    unsigned sizeAfterEncode() const;

    bool isNull(int n) const;

    unsigned calcNullsIndicatorSize() const;

    void calcAttrsSizes(const byte *attrsData);

    unsigned calcSizeAttrValue(int n, const byte *itAttr);

    RC printRecord();

private:
    const byte *_data;
    const vector<Attribute>& _attrs;
    vector<unsigned> _attrsSizes;
    unsigned _totalAttrsSizes;
};


#endif
