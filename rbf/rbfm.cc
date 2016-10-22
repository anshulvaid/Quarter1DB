#include "rbfm.h"
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <memory>

////////////////// Ugly hack to circumvent problem of having a fixed makefile
#include "RecordEncoder.h"
#include "RecordEncoder.cc"
#include "RecordDecoder.h"
#include "RecordDecoder.cc"
#include "RCEncoder.h"
#include "RCEncoder.cc"
#include "RCDecoder.h"
#include "RCDecoder.cc"
#include "Page.h"
#include "Page.cc"

#include "utils.h"
#define LOG(msg) __LOG__("RecordFileManager", msg)



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


    shared_ptr<RecordEncoder> re = make_shared<RecordEncoder>
                                            ((byte *) data, recordDescriptor);
    RCEncoder rc(re);

    int pageNum;
    Page *p = findPageToInsert(fileHandle, rc.sizeAfterEncode(), &pageNum);
    if (p != NULL) {
        LOG("Inserting record into page " << pageNum);
        rid.pageNum = pageNum;
        rid.slotNum = p->insertRecord(rc);
        fileHandle.writePage(pageNum, p->getData());
        delete p;
        return 0;
    }
    else {
        LOG("Inserting record into a new page");
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
        if (fileHandle.readPage(nPages-1, p->getData()) != -1) {
            if (p->canStoreRecord(sizeRecord)) {
                *pageNum = nPages-1;
                return p;
            }
        }


        // Start from the first page
        for (int i = 0; i < nPages- 1; ++i) {
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
    LOG("Reading record with rid (" << rid.pageNum << ", " << rid.slotNum <<
        ")    (page, slot)");

    Page p;         // allocate memory to read the page
    Maybe<RCDecoder> rc = findRecord(p, fileHandle, recordDescriptor, rid);
    if (rc) {
        if ((*rc).hasAnotherRID()) {
            return readRecord(fileHandle, recordDescriptor,
                              (*rc).decodeNextRID(), data);
        }
        (*rc).decode((byte *) data);
        return 0;
    }
    return -1;
}

Maybe<RCDecoder> RecordBasedFileManager::findRecord(
                                    const Page& allocPage,
                                    FileHandle& fileHandle,
                                    const vector<Attribute>& recordDescriptor,
                                    const RID& rid) {
    Maybe<RCDecoder> result;

    if (fileHandle.readPage(rid.pageNum, allocPage.getData()) != -1) {
        byte *rcAddr;
        unsigned rcSize;
        if (allocPage.readRecord(rid.slotNum, &rcAddr, &rcSize) != -1) {
            shared_ptr<RecordDecoder> rd
                                = make_shared<RecordDecoder>(recordDescriptor);
            result = RCDecoder(rcAddr, rcSize, rd);
        }
    }

    return result;
}


RC RecordBasedFileManager::printRecord(
    const vector<Attribute>& recordDescriptor,
    const void *data) {
    RecordEncoder re((byte *) data, recordDescriptor);
    return re.printRecord();
}



RC RecordBasedFileManager::deleteRecord(FileHandle& fileHandle,
                const vector<Attribute>& recordDescriptor,
                const RID& rid) {

    Page p;         // allocate memory to read the page
    Maybe<RCDecoder> rc = findRecord(p, fileHandle, recordDescriptor, rid);
    if (rc) {
        if ((*rc).hasAnotherRID()) {
            RID nextRID = (*rc).decodeNextRID();
            p.deleteRecord(rid.slotNum);
            return deleteRecord(fileHandle, recordDescriptor, nextRID);
        }
        p.deleteRecord(rid.slotNum);
        return 0;
    }
    return -1;
}



RC RecordBasedFileManager::readAttribute(FileHandle& fileHandle,
                                     const vector<Attribute>& recordDescriptor,
                                     const RID& rid,
                                     const string& attributeName,
                                     void *data) {
    Page p;         // allocate memory to read the page
    Maybe<RCDecoder> rc = findRecord(p, fileHandle, recordDescriptor, rid);
    if (rc) {
        if ((*rc).hasAnotherRID()) {
            return readAttribute(fileHandle, recordDescriptor,
                              (*rc).decodeNextRID(), attributeName, data);
        }

        return (*rc).decodeAttr((byte *) data, recordDescriptor, attributeName);
    }
    return -1;
}
