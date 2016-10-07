#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "pfm.h"

// Ugly hack to circumvent problem of having a fixed makefile
#include "FileHandle.h"
#include "FileHandle.cc"


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

