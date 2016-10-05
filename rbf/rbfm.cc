#include "rbfm.h"
#include <string.h>

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
    return fromByteArray(getLastNthByteAddr(3), 2);
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
    write(getFreeSpaceOffsetAddr(), toByteArray(offset), 2);
}

void Page::setNumberSlots(int n) {
    write(getNumberSlotsAddr(), toByteArray(n), 2);
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

char *Page::toByteArray(unsigned value) {
    for (int i = 0; i < sizeof(_auxByteArray); ++i) {
        _auxByteArray[i] = value & 0xFF;
        value >>= 8;
    }
    return _auxByteArray;
}

unsigned Page::fromByteArray(char *arr, size_t n) {
    unsigned res = 0;
    for (int i = 0; i < n; ++i) {
        res |= (arr[i] << i*8);
    }
    return res;
}

void Page::write(char *dest, char *data, size_t n) {
    memcpy(dest, data, n);
}

unsigned Page::freeSpace() {
    return -1;
}



////////////////////////////////////////////////////////////////////////////
/// Class Record
////////////////////////////////////////////////////////////////////////////

