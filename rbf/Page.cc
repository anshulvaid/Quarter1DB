///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _page_cc_
#define _page_cc_

#include <cassert>
#include "utils.h"
#ifndef LOG   // avoid overwriting definition
    #define LOG(msg) __LOG__("PAGE", msg)
#endif


Page::Page() {
    _data = (byte *) malloc(PAGE_SIZE * sizeof(byte));
    reset();
}

Page::Page(byte *data) : _data(data) {
}

Page::~Page() {
    delete _data;
}



/////////////////////////
// Public functions
/////////////////////////

unsigned Page::getFreeSpace() {
    // byte *lastSlotAddr = getNumberSlots() > 0 ? getLastSlotAddr() :
    //                                             getNumberSlotsAddr();
    return getLastSlotAddr() - getFreeSpaceAddr();
}


unsigned Page::insertRecord(const RecordEncoder& re) {
    LOG("Inserting new record of size " << re.sizeAfterEncode()
        << " bytes into address " << (void *) getFreeSpaceAddr()
        << " (offset = " << getFreeSpaceAddr() - _data << ")");

    byte *freeSpaceAddr = getFreeSpaceAddr();
    re.encode(freeSpaceAddr);
    insertSlot(getFreeSpaceOffset(), re.sizeAfterEncode());
    setFreeSpaceOffset(getFreeSpaceOffset() + re.sizeAfterEncode());
    return getNumberSlots() - 1;
}


RC Page::readRecord(unsigned slotNum, byte **recordAddr, unsigned *recordSize){
    LOG("Read record on slot " << slotNum);

    if (getNumberSlots() >= slotNum) {
        *recordAddr = getRecordAddr(slotNum);
        LOG("Record starts at the address " << (void *) *recordAddr);
        LOG("Page starts at address " << (void *) _data);
        *recordSize = getRecordSize(slotNum);
        LOG("Record has size " << *recordSize);
        return 0;
    }

    return -1;
}


bool Page::canStoreRecord(int size) {
    return getFreeSpace() >= size + 4;
}


void Page::reset() {
    LOG("Resetting page");
    setFreeSpaceOffset(0);
    setNumberSlots(0);
}





/////////////////////////
// Private functions
/////////////////////////


// Insert slot
void Page::insertSlot(unsigned recordOffset, unsigned recordSize) {
    LOG("Inserting a new slot with offset " << recordOffset
        << " and size " << recordSize << " bytes");

    write(getLastSlotAddr() - 4, ByteArray::encode(recordOffset), 2);
    write(getLastSlotAddr() - 2, ByteArray::encode(recordSize), 2);
    setNumberSlots(getNumberSlots() + 1);
}


// Record accessors
unsigned Page::getRecordSize(unsigned slotNum) {
    LOG("Address of slot " << slotNum << " is " <<
           (void *) getNthSlotAddr(slotNum));
    return ByteArray::decode(getNthSlotAddr(slotNum) + 2, 2);
}

unsigned Page::getRecordOffset(unsigned slotNum) {
    return ByteArray::decode(getNthSlotAddr(slotNum), 2);
}

byte *Page::getRecordAddr(unsigned slotNum) {
    return _data + getRecordOffset(slotNum);
}


// Slots accessors
unsigned Page::getNumberSlots() {
    return ByteArray::decode(getLastNthByteAddr(3), 2);
}

byte *Page::getLastSlotAddr() {
    unsigned n = getNumberSlots();
    // assert(n > 0 && "No slots were assigned yet");
    return getNthSlotAddr(n - 1);
}

// byte *Page::getFutureSlotAddr() {
//     return ;
// }

byte *Page::getNthSlotAddr(int slotNum) {
    return getLastNthByteAddr(slotNum * 4 + 7);
}

byte *Page::getNumberSlotsAddr() {
    return getLastNthByteAddr(3);
}


// Slots modifiers
void Page::setNumberSlots(unsigned n) {
    LOG("Setting number of slots to " << n);
    write(getNumberSlotsAddr(), ByteArray::encode(n), 2);
}


// Free space accessors
unsigned Page::getFreeSpaceOffset() {
    return ByteArray::decode(getFreeSpaceOffsetAddr(), 2);
}

byte *Page::getFreeSpaceOffsetAddr() {
    return getLastNthByteAddr(1);
}

byte *Page::getFreeSpaceAddr() {
    LOG("getFreeSpaceOffset() = " << getFreeSpaceOffset());
    return _data + getFreeSpaceOffset();
}


// Free space modifiers
void Page::setFreeSpaceOffset(unsigned offset) {
    write(getFreeSpaceOffsetAddr(), ByteArray::encode(offset), 2);
}


// Data accessors
byte *Page::getNthByteAddr(unsigned n) {
    return _data + n;
}

byte *Page::getLastNthByteAddr(unsigned n) {
    return _data + PAGE_SIZE - 1 - n;
}


// Data modifiers
void Page::write(byte *dst, byte *data, size_t n) {
    memcpy(dst, data, n);
}

byte *Page::getData() {
    return _data;
}

#undef LOG

#endif