#ifndef _RCEncoder_h_
#define _RCEncoder_h_

#include <memory>
#include "pfm.h"

using namespace std;


// This class contains the logic for encoding Record Containers (RC), which
// represent record entries within a page. The data associated with a record
//  container in a page has a header and a body. The header tells us
// whether the actual record data exists in a different page and, if so, it
// knows the page number.
// It encapsulates the logic of record encoders, in case the
// actual record data is to be stored in the current page. This way, the page
// doesn't need to know about the format of each individual record.
class RCEncoder {
public:
    // Constructors
    RCEncoder(const RID& rid);

    RCEncoder(shared_ptr<RecordEncoder> re);

    // Encode record container into the given destination pointer
    void encode(byte *dst) const;

    // Size of the encoded record container
    unsigned sizeAfterEncode() const;


private:
    shared_ptr<RecordEncoder> _re;

    // In case the record is in a different page
    RID _rid;
    bool _isInAnotherPage;
};

#endif