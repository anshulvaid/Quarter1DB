#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

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
    if(fileExists(fileName) && !fileHandle.isHandlingFile()) {
        return fileHandle.openFile(fileName);
    }

    return -1;
}


RC PagedFileManager::closeFile(FileHandle& fileHandle) {
    if(fileHandle.isHandlingFile()) {
        return fileHandle.closeFile();
    }
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
    if (isHandlingFile()) {
        if (pageNum < _nPages) {
            _fs.seekg(pageNum * PAGE_SIZE);
            _fs.read((char *) data, PAGE_SIZE);
            readPageCounter++;
            return 0;
        }
    }

    return -1;
}


RC FileHandle::writePage(PageNum pageNum, const void *data) {
    if (isHandlingFile()) {
        if (pageNum < _nPages) {
            _fs.seekg(pageNum * PAGE_SIZE);
            _fs.write((char *) data, PAGE_SIZE);
            _fs.flush();
            writePageCounter++;
            return 0;
        }
    }

    return -1;
}


RC FileHandle::appendPage(const void *data) {
    if (isHandlingFile()) {
        appendPageCounter++;
        return writePage(_nPages++, data);
    }

    return -1;
}


unsigned FileHandle::getNumberOfPages() {
    assert(isHandlingFile() && "File handle is not yet handling a file");
    return _nPages;
}


RC FileHandle::collectCounterValues(unsigned& readPageCount,
                                    unsigned& writePageCount,
                                    unsigned& appendPageCount) {
    readPageCount = readPageCounter;
    writePageCount = writePageCounter;
    appendPageCount = appendPageCounter;
    return 0;
}


RC FileHandle::openFile(const string& fileName) {
    if (!isHandlingFile()) {
        _fs.open(fileName);
        if (_fs.good()) {
            _nPages = calcNumberPages();
            return 0;
        }
        _fs.close();
    }

    return -1;
}


RC FileHandle::closeFile() {
    if (isHandlingFile()) {
        _fs.close();
    }

    return 0;
}


bool FileHandle::isHandlingFile() {
    return _fs.is_open() && _fs.good();
}


unsigned FileHandle::calcNumberPages() {
    assert(isHandlingFile() && "File handle is not yet handling a file");
    return (unsigned) ceil( (double) getFileSize() / PAGE_SIZE);
}


long FileHandle::getFileSize() {
    assert(isHandlingFile() && "File handle is not yet handling a file");

    long begin, end;
    _fs.seekg(0, ios::beg);
    begin = _fs.tellg();
    _fs.seekg(0, ios::end);
    end = _fs.tellg();

    return end - begin;
}
