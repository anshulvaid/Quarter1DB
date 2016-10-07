#ifndef _page_h_
#define _page_h_

class RecordEncoder;


class Page {
public:
    Page();
    Page(char *data);

    ~Page();

    // Amount of free space in bytes
    unsigned getFreeSpace();

    // Number of slots in the page == number of records
    unsigned getNumberSlots();

    unsigned insertRecord(const RecordEncoder& re);

    void insertSlot(unsigned recordOffset, unsigned recordSize);

    RC getRecord(unsigned slotNum, char **recordAddr, unsigned *recordSize);

    char *getRecordAddr(unsigned slotNum);

    unsigned getRecordSize(unsigned slotNum);

    unsigned getRecordOffset(unsigned slotNum);

    char *getNthSlotAddr(unsigned slotNum);

    char *getData();

    // Reset page format. Clears all data
    void reset();

    void setFreeSpaceOffset(unsigned offset);

    char *getLastSlotAddr();

    void setNumberSlots(int n);

    bool canStoreRecord(int size);

    char *getFreeSpaceOffsetAddr();

    char *getFreeSpaceAddr();

    unsigned getFreeSpaceOffset();

    char *getNumberSlotsAddr();

    char *getNthByteAddr(int n);

    char *getLastNthByteAddr(int n);

    char *toByteArray(unsigned value);

    unsigned fromByteArray(char *arr, size_t n);

    // Write 'n' bytes from the memory address 'data' to the given
    // destination address
    void write(char *dst, char *data, size_t n);

    // Return amount of free space in bytes
    unsigned freeSpace();

private:
    char *_data;
};

#endif