#ifndef _recordDecoder_h_
#define _recordDecoder_h_


class RecordDecoder{
public:
    RecordDecoder(byte *data, unsigned size,
                  const vector<Attribute>& recordDescriptor);

    void decode(byte *dst);
private:
    byte *_data;
    unsigned _size;
    const vector<Attribute>& _attrs;

    unsigned decodeHeader(byte *dst);
    vector<bool> getNullAttrs();
    unsigned decodeNullsIndicator(byte *dst,
                                  const vector<bool>& nullsIndicator);
    unsigned decodeBody(byte *dst);

};


#endif
