#ifndef _recordEncoder_h_
#define _recordEncoder_h_

class RecordEncoder {
public:
    RecordEncoder(const char *data, const vector<Attribute>& attrs);

    void encode(char *dst) const;

    // Returns the number of bytes written
    unsigned encodeHeader(char *dst) const;

    // Returns the number of bytes written
    unsigned encodeBody(char *dst) const;

    unsigned sizeAfterEncode() const;

    bool isNull(int n) const;

    unsigned calcNullsIndicatorSize() const;

    void calcAttrsSizes(const char *attrsData);

    unsigned calcSizeAttrValue(int n, const char *itAttr);
private:
    const char *_data;
    const vector<Attribute>& _attrs;
    vector<unsigned> _attrsSizes;
    unsigned _totalAttrsSizes;
};


#endif
