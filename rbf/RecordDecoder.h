#ifndef _recordDecoder_h_
#define _recordDecoder_h_


class RecordDecoder{
public:
    RecordDecoder(char *data, unsigned size,
                  const vector<Attribute>& recordDescriptor);

    void decode(char *dst);
private:
    char *_data;
    unsigned _size;
    const vector<Attribute>& _attrs;

    unsigned decodeHeader(char *dst);
    unsigned decodeNullsIndicator(char *dst,
                                  const vector<bool>& nullsIndicator);
    unsigned decodeBody(char *dst);

};


#endif
