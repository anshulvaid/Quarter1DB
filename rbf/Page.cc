///////////////////////////////////////////////////////////////////////
///// File included by rbfm.h (circumvent fixed makefile problem)
///////////////////////////////////////////////////////////////////////


#ifndef _page_cc_
#define _page_cc_


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

#endif