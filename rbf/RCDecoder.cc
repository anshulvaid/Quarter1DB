///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _RCDecoder_cc_
#define _RCDecoder_cc_

#include <cassert>
#include "utils.h"
#ifndef LOG   // avoid overwriting definition
    #define LOG(msg) __LOG__("RCDecoder", msg)
#endif


RCDecoder::RCDecoder() : _header(NULL), _body(NULL), _size(0) {
}

RCDecoder::RCDecoder(byte *data, unsigned size, shared_ptr<RecordDecoder> rd)
                : _header(data), _size(size), _rd(rd){
    if (hasAnotherRID()) {
        _body = NULL;
    }
    else {
        _body = _header + 1;
    }
}


RID RCDecoder::decodeNextRID() {
    assert(hasAnotherRID() && "This record is not stored in another page!");

    unsigned pageNum = ByteArray::decode(_header, 4) & 0x7FFFFFFF;
    unsigned slotNum = ByteArray::decode(_header + 4, 2);
    return {pageNum, slotNum};
}


void RCDecoder::decode(byte *dst) {
    assert(!hasAnotherRID());

    _rd->setRecordData(_body, _size - (_body - _header));
    return _rd->decode(dst);
}


RC RCDecoder::decodeAttr(byte *dst,
                           const vector<Attribute>& recordDescriptor,
                           const string& attributeName) {
    return _rd->decodeAttr(dst, recordDescriptor, attributeName);
}


inline bool RCDecoder::hasAnotherRID() {
    assert(_header != NULL);
    return ((*_header & 0x80000000) != 0);
}



#undef LOG

#endif
