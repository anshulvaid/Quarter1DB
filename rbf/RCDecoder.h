#ifndef _RCDecoder_h_
#define _RCDecoder_h_

#include <memory>
#include "pfm.h"

using namespace std;


// This class contains the logic for decoding Record Containers (RC), which
// represent record entries within a page. The data associated with a record
//  container in a page has a header and a body. The header tells us
// whether the actual record data exists in a different page and, if so, it
// knows the page number.
// It encapsulates the logic of record decoders, in case the
// actual record data is stored in the current page. This way, the page
// doesn't need to know about the format of each individual record.
class RCDecoder {
public:
    // Constructors
    RCDecoder(byte *data, unsigned size, shared_ptr<RecordDecoder> rd);

    inline bool hasAnotherRID();
    RID decodeNextRID();
    void decode(byte *dst);

private:
    byte *_header;  // Contains information to whether the record is in the
                    // current page
    byte *_body;    // Contains the actual record
    unsigned _size;     // Size of the entire record container (inc. header)

    shared_ptr<RecordDecoder> _rd;
};

#endif