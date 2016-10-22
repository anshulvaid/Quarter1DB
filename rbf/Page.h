#ifndef _page_h_
#define _page_h_

class RCEncoder;


class Page {
public:
    Page();
    Page(byte *data);

    ~Page();

    // Amount of free space in bytes
    unsigned getFreeSpace() const;

    // Insert record. Returns the slot ID (starting at 0)
    // for the newly inserted record
    unsigned insertRecord(const RCEncoder& rc);

    // Read record into given pointer address and set its size
    RC readRecord(unsigned slotNum,
                  byte **recordAddr, unsigned *recordSize) const;

    bool canStoreRecord(int size) const;

    // Reset page format. Clears all data
    void reset();

    byte *getData() const;

protected:   // for testing purposes

    // Record accessors
    unsigned getRecordSize(unsigned slotNum) const;
    unsigned getRecordOffset(unsigned slotNum) const;
    byte *getRecordAddr(unsigned slotNum) const;


    // Slots accessors
    unsigned getNumberSlots() const;  // == number of records
    byte *getLastSlotAddr() const;
    byte *getNthSlotAddr(int slotNum) const;
    byte *getNumberSlotsAddr() const;
    inline bool slotCanBeReused(unsigned slotNum) const;

    // Slots modifiers
    void setNumberSlots(unsigned n);
    void insertSlot(unsigned recordOffset, unsigned recordSize);
    void setSlot(unsigned slotNum, unsigned recordOffset, unsigned recordSize);


    // Free space accessors
    unsigned getFreeSpaceOffset() const;
    byte *getFreeSpaceOffsetAddr() const;
    byte *getFreeSpaceAddr() const;

    // Free space modifiers
    void setFreeSpaceOffset(unsigned offset);


    // Data accessors
    byte *getNthByteAddr(unsigned n) const;
    byte *getLastNthByteAddr(unsigned n) const;

    // Data modifiers
    // Write 'n' bytes from the memory address 'data' to the given
    // destination address
    void write(byte *dst, byte *data, size_t n);

protected:
    byte *_data;
};

#endif