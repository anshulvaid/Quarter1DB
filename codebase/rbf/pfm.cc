#include "pfm.h"
#include <iostream>
#include <fstream>

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
}


RC PagedFileManager::createFile(const string &fileName)
{
	fstream createHandle;
	int success =0;
	createHandle.open(fileName.c_str(), ios::binary | ios::in);

	//if the above operation failed, file doesn't exit; go ahead with creation
	if(!createHandle){
		createHandle.open(fileName.c_str(), ios::binary | ios::out | ios:: trunc);
		createHandle.close();
	}
	//if the file exists already, return failure
	else{
		success = -1;
	}
    return success;
}


RC PagedFileManager::destroyFile(const string &fileName)
{
	if(remove(fileName.c_str()) == 0){
		return 0;
	}
    return -1;
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
	int success = 0;

	//use the handler object from FileHandle class
	//check if handler is already bound to another file
	if(fileHandle.handler->is_open()){
		success = -1;
	}
	//if no, associate the handler with the mentioned file
	else{
		fileHandle.handler->open(fileName.c_str(), ios::binary | ios::in | ios::out | ios::ate);
	}
    return success;
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
	if(fileHandle.handler->is_open()){

		//write the contents of the stream to the file
		fileHandle.handler->flush();
		fileHandle.handler->close();

		//destroy the handler
		delete fileHandle.handler;
		return 0;
	}
    return -1;
}


FileHandle::FileHandle()
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
    handler = new fstream();
}


FileHandle::~FileHandle()
{
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
	unsigned numOfPages = getNumberOfPages();
	int success = 0;
	if(pageNum >= numOfPages){
		success = -1;
	}else{
		handler->seekg(pageNum * PAGE_SIZE, ios_base::beg);
		if(handler->read((char*)data,PAGE_SIZE)){
			readPageCounter++;
			success = 0;;
		}else{
			success = -1;
		}
	}
	return success;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
	unsigned numOfPages = getNumberOfPages();
	int success = 0;
	if(pageNum >= numOfPages){
		success = -1;
	}else{
		handler->seekg(pageNum * PAGE_SIZE, ios_base::beg);
		if(handler->write((char*)data,PAGE_SIZE)){
			writePageCounter++;
			success = 0;;
		}else{
			success = -1;
		}

	}
    return success;
}


RC FileHandle::appendPage(const void *data)
{
	int success = -1;
	handler->seekg(0, ios::end);
	if(handler->write((char*)data,PAGE_SIZE)){
		appendPageCounter++;
		success = 0;;
	}
	return success;
}


unsigned FileHandle::getNumberOfPages()
{
	handler->seekg(0, ios::end);
	long fileSize = handler->tellg();
    return fileSize/PAGE_SIZE;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
	readPageCount = readPageCounter;
	writePageCount = writePageCounter;
	appendPageCount = appendPageCounter;
    return 0;
}
