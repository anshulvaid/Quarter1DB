#include "rbfm.h"
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance() {
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager() : 
    _pfm(*PagedFileManager::instance()) {
}

RecordBasedFileManager::~RecordBasedFileManager() {
}

RC RecordBasedFileManager::createFile(const string& fileName) {
    return _pfm.createFile(fileName);
}

RC RecordBasedFileManager::destroyFile(const string& fileName) {
    return _pfm.destroyFile(fileName);
}

RC RecordBasedFileManager::openFile(const string& fileName,
                                    FileHandle& fileHandle) {
    return _pfm.openFile(fileName, fileHandle);
}

RC RecordBasedFileManager::closeFile(FileHandle& fileHandle) {
    return _pfm.closeFile(fileHandle);
}

RC RecordBasedFileManager::insertRecord(
    FileHandle& fileHandle,
    const vector<Attribute>& recordDescriptor,
    const void *data,
    RID& rid) {


    RecordEncoder re((char *) data, recordDescriptor);

    int pageNum;
    Page *p = findPageToInsert(fileHandle, re.sizeAfterEncode(), &pageNum);
    if (p != NULL) {
        rid.pageNum = pageNum;
        rid.slotNum = p->insertRecord(re);
        delete p;
        return 0;
    }
    else {
        {
            Page p;
            fileHandle.appendPage(p.getData());
        }
        return insertRecord(fileHandle, recordDescriptor, data, rid);
    }

    return -1;
}

Page* RecordBasedFileManager::findPageToInsert(FileHandle& fileHandle,
                                               int sizeRecord,
                                               int *pageNum) {
    unsigned nPages = fileHandle.getNumberOfPages();
    if (nPages > 0) {
        Page *p = new Page();
        // Start with the last page
        for (int i = nPages-1; i >= 0; --i) {
            if (fileHandle.readPage(i, p->getData()) != -1) {
                if (p->canStoreRecord(sizeRecord)) {
                    *pageNum = i;
                    return p;
                }
            }
        }
    }

    // Either there are no pages yet, or all of them are full
    return NULL;
}

RC RecordBasedFileManager::readRecord(
    FileHandle& fileHandle,
    const vector<Attribute>& recordDescriptor,
    const RID& rid,
    void *data) {

    Page p;

    if (fileHandle.readPage(rid.pageNum, p.getData()) != -1) {
        char *recordAddr;
        unsigned recordSize;
        if (p.getRecord(rid.slotNum, &recordAddr, &recordSize) != -1) {
            RecordDecoder rd(recordAddr, recordSize, recordDescriptor);
            rd.decode((char *) data);
            return 0;
        }
    }

    return -1;
}

RC RecordBasedFileManager::printRecord(
    const vector<Attribute>& recordDescriptor,
    const void *data) {
    return -1;
}


////////////////////////////////////////////////////////////////////////////
/// Class ByteArray
////////////////////////////////////////////////////////////////////////////


class ByteArray {
public:
    static char *encode(unsigned value) {
        for (int i = 0; i < sizeof(value); ++i) {
            _auxByteArray[i] = value & 0xFF;
            value >>= 8;
        }
        return _auxByteArray;
    }

    static unsigned decode(const char *arr, size_t n) {
        unsigned res = 0;
        for (int i = 0; i < n; ++i) {
            res |= (arr[i] << i*8);
        }
        return res;
    }

    static char _auxByteArray[sizeof(unsigned)];
};

char ByteArray::_auxByteArray[sizeof(unsigned)];





////////////////////////////////////////////////////////////////////////////
/// Class Page
////////////////////////////////////////////////////////////////////////////

Page::Page() {
    _data = (char *) malloc(PAGE_SIZE * sizeof(char));
    reset();
}

Page::Page(char *data) : _data(data) {
}

Page::~Page() {
    delete _data;
}

unsigned Page::getFreeSpace() {
    return getLastSlotAddr() - getFreeSpaceAddr();
}

char *Page::getLastSlotAddr() {
    return getLastNthByteAddr(4 + 4 * getNumberSlots() - 1);
}

unsigned Page::getNumberSlots() {
    return ByteArray::decode(getLastNthByteAddr(3), 2);
}

bool Page::canStoreRecord(int size) {
    return getFreeSpace() >= size + 4;
}


void Page::reset() {
    setFreeSpaceOffset(0);
    setNumberSlots(0);
}

char *Page::getFreeSpaceAddr() {
    return _data + getFreeSpaceOffset();
}

unsigned Page::insertRecord(const RecordEncoder& re) {
    char *freeSpaceAddr = getFreeSpaceAddr();
    re.encode(freeSpaceAddr);
    insertSlot(getFreeSpaceOffset(), re.sizeAfterEncode());
    setFreeSpaceOffset(getFreeSpaceOffset() + re.sizeAfterEncode());
    return getNumberSlots();
}

RC Page::getRecord(unsigned slotNum, char **recordAddr, unsigned *recordSize){
    if (getNumberSlots() >= slotNum) {
        *recordAddr = getRecordAddr(slotNum);
        *recordSize = getRecordSize(slotNum);
        return 0;
    }

    return -1;
}

char *Page::getRecordAddr(unsigned slotNum) {
    return _data + getRecordOffset(slotNum);
}

unsigned Page::getRecordSize(unsigned slotNum) {
    return ByteArray::decode(getNthSlotAddr(slotNum) + 2, 2);
}

unsigned Page::getRecordOffset(unsigned slotNum) {
    return ByteArray::decode(getNthSlotAddr(slotNum), 2);
}

char *Page::getNthSlotAddr(unsigned slotNum) {
    return getLastNthByteAddr(slotNum * 4 + 3);
}


// Private functions
void Page::insertSlot(unsigned recordOffset, unsigned recordSize) {
    write(getLastSlotAddr() - 4, ByteArray::encode(recordOffset), 2);
    write(getLastSlotAddr() - 2, ByteArray::encode(recordSize), 2);
    setNumberSlots(getNumberSlots() + 1);

    //
}

void Page::setFreeSpaceOffset(unsigned offset) {
    write(getFreeSpaceOffsetAddr(), ByteArray::encode(offset), 2);
}

unsigned Page::getFreeSpaceOffset() {
    return ByteArray::decode(getFreeSpaceOffsetAddr(), 2);
}

void Page::setNumberSlots(int n) {
    write(getNumberSlotsAddr(), ByteArray::encode(n), 2);
}

char *Page::getFreeSpaceOffsetAddr() {
    return getLastNthByteAddr(1);
}

char *Page::getNumberSlotsAddr() {
    return getLastNthByteAddr(3);
}

char *Page::getNthByteAddr(int n) {
    return _data + n;
}

char *Page::getLastNthByteAddr(int n) {
    return _data + PAGE_SIZE - 1 - n;
}

void Page::write(char *dst, char *data, size_t n) {
    memcpy(dst, data, n);
}

char *Page::getData() {
    return _data;
}

unsigned Page::freeSpace() {
    return -1;
}



////////////////////////////////////////////////////////////////////////////
/// Class RecordEncoder
////////////////////////////////////////////////////////////////////////////

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




////////////////////////////////////////////////////////////////////////////
/// Class RecordDecoder
////////////////////////////////////////////////////////////////////////////

RecordDecoder::RecordDecoder(char *data, unsigned size,
                             const vector<Attribute>& recordDescriptor)
        : _data(data),
          _size(size),
          _attrs(recordDescriptor){

}

void RecordDecoder::decode(char *dst) {
    dst += decodeHeader(dst);
    dst += decodeBody(dst);
}

unsigned RecordDecoder::decodeHeader(char *dst) {
    vector<bool> nullsIndicator(_attrs.size());
    for (int i = 0; i < _attrs.size(); ++i) {
        unsigned attrOffset = ByteArray::decode(_data + 2 * i, 2);
        nullsIndicator[i] = (attrOffset == 0);
    }

    return decodeNullsIndicator(dst, nullsIndicator);
}

unsigned RecordDecoder::decodeNullsIndicator(char *dst,
                                        const vector<bool>& nullsIndicator) {
    byte b, mask;
    char *writePos = dst;
    for (int i = 0; i < _attrs.size(); ++i) {
        if (i % 8 == 0) {
            mask = 0x80;
            b = 0;

            if (i != 0) {
                *writePos++ = b;
            }
        }

        if (nullsIndicator[i]) {
            b |= mask;
        }
        mask >>= 1;
    }

    return writePos - dst;
}


unsigned RecordDecoder::decodeBody(char *dst) {
    unsigned headerSize = 2 * _attrs.size();
    unsigned bodySize = _size - headerSize;
    memcpy(dst, _data + headerSize, bodySize);
    return bodySize;
}
