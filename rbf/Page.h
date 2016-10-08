#ifndef _page_h_
#define _page_h_

class RecordEncoder;


class Page {
public:
    Page();
    Page(byte *data);

    ~Page();

    // Amount of free space in bytes
    unsigned getFreeSpace();

    // Insert record
    unsigned insertRecord(const RecordEncoder& re);

    // Read record into given pointer address and set its size
    RC readRecord(unsigned slotNum, byte **recordAddr, unsigned *recordSize);

    bool canStoreRecord(int size);

    // Reset page format. Clears all data
    void reset();

    byte *getData();

protected:   // for testing purposes
    // Insert slot
    void insertSlot(unsigned recordOffset, unsigned recordSize);

    // Record accessors
    unsigned getRecordSize(unsigned slotNum);
    unsigned getRecordOffset(unsigned slotNum);
    byte *getRecordAddr(unsigned slotNum);


    // Slots accessors
    unsigned getNumberSlots();  // == number of records
    byte *getLastSlotAddr();
    byte *getNthSlotAddr(int slotNum);
    byte *getNumberSlotsAddr();

    // Slots modifiers
    void setNumberSlots(unsigned n);


    // Free space accessors
    unsigned getFreeSpaceOffset();
    byte *getFreeSpaceOffsetAddr();
    byte *getFreeSpaceAddr();

    // Free space modifiers
    void setFreeSpaceOffset(unsigned offset);


    // Data accessors
    byte *getNthByteAddr(unsigned n);
    byte *getLastNthByteAddr(unsigned n);

    // Data modifiers
    // Write 'n' bytes from the memory address 'data' to the given
    // destination address
    void write(byte *dst, byte *data, size_t n);

protected:
    byte *_data;
};

#endif