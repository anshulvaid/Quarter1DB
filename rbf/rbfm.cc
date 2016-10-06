#include "rbfm.h"
#include <string.h>
#include <math.h>
#include <vector>
#include <numeric>

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


    // if getPage() {

    // }
    // else {
    //     Page p;
    //     fileHandle.append(p.getData());
    //     return insertRecord();
    // }

 
    return -1;
}

// Page* RecordBasedFileManager::getPage(FileHandle fileHandle, int nBytes) {
//     unsigned nPages = fileHandle.getNumberOfPages();
//     if (nPages > 0) {
//         Page *p = new Page();
//         // Start with the last page
//         for (int i = nPages-1; i >= 0; --i) {
//             if (fileHandle.readPage(i, data) != -1) {
//                 if (p->freeSpace() >= nBytes) {
//                     return p;
//                 }
//             }
//         }
//     }

//     // Either there are no pages yet, or all of them are full
//     return NULL;
// }

// void RecordBasedFileManager::createNewPage() {

// }

RC RecordBasedFileManager::readRecord(
    FileHandle& fileHandle,
    const vector<Attribute>& recordDescriptor,
    const RID& rid,
    void *data) {
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

    static unsigned decode(char *arr, size_t n) {
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
    return getLastNthByteAddr(2 * (getNumberSlots() + 2) - 1)
           - getFreeSpaceOffsetAddr();
}

unsigned Page::getNumberSlots() {
    return ByteArray::decode(getLastNthByteAddr(3), 2);
}

bool Page::canStoreRecord(int size) {
    return getFreeSpace() >= size + 2;
}

// Private functions

void Page::reset() {
    setFreeSpaceOffset(0);
    setNumberSlots(0);
}

void Page::setFreeSpaceOffset(unsigned offset) {
    write(getFreeSpaceOffsetAddr(), ByteArray::encode(offset), 2);
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

unsigned Page::freeSpace() {
    return -1;
}



////////////////////////////////////////////////////////////////////////////
/// Class RecordEncoder
////////////////////////////////////////////////////////////////////////////

class RecordEncoder {
public:
    RecordEncoder(char *data, const vector<Attribute>& attrs)
    : _data(data), _attrs(attrs) {
    }


    // Returns the number of bytes written
    void encode(char *dst) {
        vector<unsigned> attrsSizes = getAttrsSizes(_data
                                                + calcNullsIndicatorSize());
        dst += encodeHeader(dst, attrsSizes);
        dst += encodeBody(dst, attrsSizes);
    }

    // Returns the number of bytes written
    unsigned encodeHeader(char *dst, const vector<unsigned>& attrsSizes) {
        unsigned nAttrs = _attrs.size();
        unsigned offset = 2 * nAttrs;
        int i;
        for (i = 0; i < nAttrs; ++i) {
            memcpy(dst + 2 * i, ByteArray::encode( isNull(i) ? 0 : offset), 2);
            offset += attrsSizes[i];
        }
        return 2 * i;
    }

    // Returns the number of bytes written
    unsigned encodeBody(char *dst, const vector<unsigned>& attrsSizes) {
        unsigned size = std::accumulate(attrsSizes.begin(),
                                        attrsSizes.end(), 0);
        memcpy(dst, _data + calcNullsIndicatorSize(), size);
        return size;
    }

    bool isNull(int n) {
        int byteOffset = n / 8;
        return (_data[byteOffset] & (1 << (8 - (n % 8) - 1))) != 0;
    }

    unsigned calcNullsIndicatorSize() {
        return ceil((double) _attrs.size() / 8);
    }

    vector<unsigned> getAttrsSizes(char *attrsData) {
        vector<unsigned> sizes(_attrs.size());
        char *it = attrsData;
        int i = 0;
        for(auto& s: sizes) {
            s = calcSizeAttrValue(i++, it);
            it += s;
        }
        return sizes;
    }

    unsigned calcSizeAttrValue(int n, char *itAttr) {
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
private:
    char *_data;
    const vector<Attribute>& _attrs;
};
