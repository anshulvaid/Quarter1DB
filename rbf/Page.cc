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

unsigned Page::getFreeSpace() const {
    return getLastSlotAddr() - getFreeSpaceAddr();
}


unsigned Page::insertRecord(const RCEncoder& rc) {
    LOG("Inserting new record of size " << rc.sizeAfterEncode()
        << " bytes into address " << (void *) getFreeSpaceAddr()
        << " (offset = " << getFreeSpaceAddr() - _data << ")");

    rc.encode(getFreeSpaceAddr());

    // Find if there's a slot that can be reused
    for (int i = 0; i < getNumberSlots(); ++i) {
        if (slotCanBeReused(i)) {
            setSlot(i, getFreeSpaceOffset(), + rc.sizeAfterEncode());
            setFreeSpaceOffset(getFreeSpaceOffset() + rc.sizeAfterEncode());
            return i;
        }
    }

    insertSlot(getFreeSpaceOffset(), rc.sizeAfterEncode());
    setFreeSpaceOffset(getFreeSpaceOffset() + rc.sizeAfterEncode());
    return getNumberSlots() - 1;
}


RC Page::readRecord(unsigned slotNum,
                    byte **recordAddr, unsigned *recordSize) const {
    LOG("Read record on slot " << slotNum);

    if (getNumberSlots() >= slotNum && !recordWasDeleted(slotNum)) {
        *recordAddr = getRecordAddr(slotNum);
        LOG("Record starts at the address " << (void *) *recordAddr);
        LOG("Page starts at address " << (void *) _data);
        *recordSize = getRecordSize(slotNum);
        LOG("Record has size " << *recordSize);
        return 0;
    }

    return -1;
}


RC Page::deleteRecord(unsigned slotNum) {
    LOG("Deleting record on slot " << slotNum);

    if (getNumberSlots() >= slotNum && !recordWasDeleted(slotNum)) {
        byte *recordAddr = getRecordAddr(slotNum);
        unsigned recordSize = getRecordSize(slotNum);
        unsigned toCopy = getFreeSpaceAddr() - (recordAddr + recordSize);

        // Overwrite record data by shifting back all the following
        // records in the page
        memmove(recordAddr, recordAddr + recordSize, toCopy);

        // Update offsets for all records in the page
        for (unsigned s = 0; s < getNumberSlots(); ++s) {
            // Only update the records that follow
            if (getRecordAddr(s) > recordAddr) {
                setRecordOffset(s, getRecordOffset(s) - recordSize);
            }
        }

        // Update free space offset
        setFreeSpaceOffset(getFreeSpaceOffset() - recordSize);

        // Update slot information to mark it as deleted
        setRecordOffset(slotNum, 0xFFFF);
    }

    return -1;
}


bool Page::canStoreRecord(int size) const {
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

    setSlot(getNumberSlots(), recordOffset, recordSize);
    setNumberSlots(getNumberSlots() + 1);
}


// Set ith slot
void Page::setSlot(unsigned slotNum,
                   unsigned recordOffset,
                   unsigned recordSize) {
    write(getNthSlotAddr(slotNum), ByteArray::encode(recordOffset), 2);
    write(getNthSlotAddr(slotNum) + 2, ByteArray::encode(recordSize), 2);
}


// Record accessors
unsigned Page::getRecordSize(unsigned slotNum) const {
    LOG("Address of slot " << slotNum << " is " <<
           (void *) getNthSlotAddr(slotNum));
    return ByteArray::decode(getNthSlotAddr(slotNum) + 2, 2);
}

unsigned Page::getRecordOffset(unsigned slotNum) const {
    return ByteArray::decode(getNthSlotAddr(slotNum), 2);
}

byte *Page::getRecordAddr(unsigned slotNum) const {
    return _data + getRecordOffset(slotNum);
}

bool Page::recordWasDeleted(unsigned slotNum) const {
    return slotCanBeReused(slotNum);
}


// Record modifiers
void Page::setRecordOffset(unsigned slotNum, unsigned offset) {
    LOG("Setting the record " << slotNum << "to offset " << offset);
    write(getNthSlotAddr(slotNum), ByteArray::encode(offset), 2);
}


// Slots accessors
unsigned Page::getNumberSlots() const {
    return ByteArray::decode(getLastNthByteAddr(3), 2);
}

byte *Page::getLastSlotAddr() const {
    unsigned n = getNumberSlots();
    // assert(n > 0 && "No slots were assigned yet");
    return getNthSlotAddr(n - 1);
}

byte *Page::getNthSlotAddr(int slotNum) const {
    return getLastNthByteAddr(slotNum * 4 + 7);
}

byte *Page::getNumberSlotsAddr() const {
    return getLastNthByteAddr(3);
}

inline bool Page::slotCanBeReused(unsigned slotNum) const {
    return getRecordOffset(slotNum) == 0xFFFF;
}

// Slots modifiers
void Page::setNumberSlots(unsigned n) {
    LOG("Setting number of slots to " << n);
    write(getNumberSlotsAddr(), ByteArray::encode(n), 2);
}


// Free space accessors
unsigned Page::getFreeSpaceOffset() const {
    return ByteArray::decode(getFreeSpaceOffsetAddr(), 2);
}

byte *Page::getFreeSpaceOffsetAddr() const {
    return getLastNthByteAddr(1);
}

byte *Page::getFreeSpaceAddr() const {
    LOG("getFreeSpaceOffset() = " << getFreeSpaceOffset());
    return _data + getFreeSpaceOffset();
}


// Free space modifiers
void Page::setFreeSpaceOffset(unsigned offset) {
    write(getFreeSpaceOffsetAddr(), ByteArray::encode(offset), 2);
}


// Data accessors
byte *Page::getNthByteAddr(unsigned n) const {
    return _data + n;
}

byte *Page::getLastNthByteAddr(unsigned n) const {
    return _data + PAGE_SIZE - 1 - n;
}


// Data modifiers
void Page::write(byte *dst, byte *data, size_t n) {
    memcpy(dst, data, n);
}

byte *Page::getData() const {
    return _data;
}

#undef LOG

#endif