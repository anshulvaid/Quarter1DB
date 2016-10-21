#ifndef _page_h_
#define _page_h_

class RCEncoder;


class Page {
public:
    Page();
    Page(byte *data);

    ~Page();

    // Amount of free space in bytes
    unsigned getFreeSpace();

    // Insert record. Returns the slot ID (starting at 0)
    // for the newly inserted record
    unsigned insertRecord(const RCEncoder& rc);

    // Read record into given pointer address and set its size
    RC readRecord(unsigned slotNum, byte **recordAddr, unsigned *recordSize);

    bool canStoreRecord(int size);

    // Reset page format. Clears all data
    void reset();

    byte *getData();

protected:   // for testing purposes

    // Record accessors
    unsigned getRecordSize(unsigned slotNum);
    unsigned getRecordOffset(unsigned slotNum);
    byte *getRecordAddr(unsigned slotNum);


    // Slots accessors
    unsigned getNumberSlots();  // == number of records
    byte *getLastSlotAddr();
    byte *getNthSlotAddr(int slotNum);
    byte *getNumberSlotsAddr();
    inline bool slotCanBeReused(unsigned slotNum);

    // Slots modifiers
    void setNumberSlots(unsigned n);
    void insertSlot(unsigned recordOffset, unsigned recordSize);
    void setSlot(unsigned slotNum, unsigned recordOffset, unsigned recordSize);


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