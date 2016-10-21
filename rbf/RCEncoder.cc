///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _RCEncoder_cc_
#define _RCEncoder_cc_

#include "utils.h"
#ifndef LOG   // avoid overwriting definition
    #define LOG(msg) __LOG__("RCEncoder", msg)
#endif


RCEncoder::RCEncoder(const RID& rid) : _rid(rid), _isInAnotherPage(true) {
}


RCEncoder::RCEncoder(shared_ptr<RecordEncoder> re) :
            _re(re), _isInAnotherPage(false) {
}




// Encode record container into the given destination pointer
void RCEncoder::encode(byte *dst) const {
    if (_isInAnotherPage) {
        // First bit of the 4 bytes used to encode the page tells us if
        // the record is in another page. (Assuming 31 bits are enough to
        // encode all pages)
        memcpy(dst, ByteArray::encode(_rid.pageNum | 0x80000000), 4);
        memcpy(dst + 4, ByteArray::encode(_rid.slotNum | 0x80000000), 2);
    }
    else {
        // Indicate that record data is in the current page
        memcpy(dst, ByteArray::encode(0x0), 1);
        _re->encode(dst + 1);
    }
}

// Size of the encoded record container
unsigned RCEncoder::sizeAfterEncode() const {
    if (_isInAnotherPage) {
        return 6;
    }

    return 1 + _re->sizeAfterEncode();
}




#undef LOG

#endif
