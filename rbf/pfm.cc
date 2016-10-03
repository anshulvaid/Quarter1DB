#include <fstream>
#include <sys/stat.h>
#include <stdio.h>

#include "pfm.h"

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance() {
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager() {
}


PagedFileManager::~PagedFileManager() {
}


RC PagedFileManager::createFile(const string& fileName) {
    if (!fileExists(fileName)) {
        if (fstream(fileName, fstream::out)) {
            return 0;
        }
    }

    return -1;
}


RC PagedFileManager::destroyFile(const string& fileName) {
    if (fileExists(fileName)) {
        return remove(fileName.c_str());
    }

    return -1;
}


RC PagedFileManager::openFile(const string& fileName, FileHandle& fileHandle) {
    if (fileExists(fileName) && !fileHandle.hasOpenedFile()) {
        return fileHandle.openFile(fileName);
    }

    return -1;
}


RC PagedFileManager::closeFile(FileHandle& fileHandle) {
    return -1;
}


bool PagedFileManager::fileExists(const string& fileName) {
    struct stat buffer;
    return (stat (fileName.c_str(), &buffer) == 0);
}


void PagedFileManager::reset() {
    delete _pf_manager;
    _pf_manager = NULL;
}


///////////////////////////////////////////////////////////////////////////
// File Handle class
///////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle() {
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
}


FileHandle::~FileHandle() {
}


RC FileHandle::readPage(PageNum pageNum, void *data) {
    return -1;
}


RC FileHandle::writePage(PageNum pageNum, const void *data) {
    return -1;
}


RC FileHandle::appendPage(const void *data) {
    return -1;
}


unsigned FileHandle::getNumberOfPages() {
    return -1;
}


RC FileHandle::collectCounterValues(unsigned& readPageCount,
                                    unsigned& writePageCount,
                                    unsigned& appendPageCount) {
    return -1;
}


RC FileHandle::openFile(const string& fileName) {
    if (!hasOpenedFile()) {
        _fs.open(fileName);
        if (_fs.good()) {
            return 0;
        }
        _fs.close();
    }

    return -1;
}


bool FileHandle::hasOpenedFile() {
    return _fs.is_open() && _fs.good();
}
