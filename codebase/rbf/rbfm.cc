#include "rbfm.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

RC RecordBasedFileManager::createFile(const string &fileName) {
	//get an instance of PagedFileManager
	PagedFileManager *pfManager = PagedFileManager::instance();
	int success = 0;

	//if create file is successful, return 0
	if(pfManager->createFile(fileName) != 0){
		success = -1;
	}
    return success;
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
	//get an instance of PagedFileManager
	PagedFileManager *pfManager = PagedFileManager::instance();
	int success = 0;

	//if destroy file is successful, return 0
	if(pfManager->destroyFile(fileName) != 0){
		success = -1;
	}
	return success;
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
	//get an instance of PagedFileManager
	PagedFileManager *pfManager = PagedFileManager::instance();
	int success = 0;

	//if open file is successful, return 0
	if(pfManager->openFile(fileName, fileHandle) != 0){
		success = -1;
	}
	return success;
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
	//get an instance of PagedFileManager
	PagedFileManager *pfManager = PagedFileManager::instance();
	int success = 0;

	//if close file is successful, return 0
	if(pfManager->closeFile(fileHandle) != 0){
		success = -1;
	}
	return success;
}

/**
 * Appends a new page to the file associated with fileHandle
 * @param fileHandle object of FileHandle class, operates on the file
 * @param page the page to be appended
 * @param data the record to be inserted
 * @param length length of the record
 * @return RC whether the operation was successful or not, 0 or 1
 */
RC AppendNewPage(FileHandle &fileHandle, void* page, const void * data, int length){

	//insert the record in the page
	memcpy((char*)page, (char*)data, length);

	//calculate the freespace left in the page after insertion of the new record
	//calulated as PAGE_SIZE - length of the new record - 3 blocks of size short int
	//the 3 blocks hold freespace in bytes, number of slots occupied,
	//the first block storing the offset of the record as the end of the record
	int freeSpace = PAGE_SIZE - length - 3*sizeof(short int), slotNum = 1;

	//insert the above mentioned three blocks in the page
	memcpy((char*)page+ (PAGE_SIZE -sizeof(short int)), &freeSpace, sizeof(short int));
	memcpy((char*)page+ (PAGE_SIZE - 2*sizeof(short int)), &slotNum, sizeof(short int));
	memcpy((char*)page+ (PAGE_SIZE - 3*sizeof(short int)), &length, sizeof(short int));

	//append the page in the file, using PFM's appendPage method
	return fileHandle.appendPage(page);
}

/**
 * Inserts a new record in a page in the file associated with fileHandle
 * @param fileHandle object of FileHandle class, operates on the file
 * @param recordDescriptor works as schema of the record
 * @param data the record to be inserted
 * @param rid returns the page number and slot number in which the record was inserted
 * @return RC whether the operation was successful or not, 0 or 1
 */
RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid) {

	//get number of attributes from recordDescriptor
	int numberOfAttributes = recordDescriptor.size();

	//get the number of bytes for the null field indicator
    int nullFieldIndicatorByteSize = ceil((double)numberOfAttributes/ CHAR_BIT);

    //to store the record(data) in the internal representation
    void *record = malloc(1000);

    unsigned char *nullFieldsIndicator = (unsigned char *) malloc(nullFieldIndicatorByteSize);
    int offset = 0, dataEntryOffset = 0, dataReadOffset = 0;

    //read null field indicator from the record and store it in nullFieldsIndicator
    memcpy(nullFieldsIndicator, (char*)data, nullFieldIndicatorByteSize);

    //position in the internal record from where the actual data begins
    //skip as many 2 bytes blocks(to store the attribute offsets) as the number of attributes,
    //and then write the actual data
    dataEntryOffset = offset + numberOfAttributes * sizeof(short int);

    //position from where the data is to be read from the param data
	dataReadOffset = nullFieldIndicatorByteSize;

    for(int i=0; i< numberOfAttributes; i++){

    	//check if the attribute is null
    	if(!(nullFieldsIndicator[i/CHAR_BIT] & (1 << (CHAR_BIT * nullFieldIndicatorByteSize - 1 - i)))){
    		switch(recordDescriptor[i].type){
    			case 0:
    				//if the attribute type is 'int'
    				//insert the attribute in the internal record
    				memcpy((char*)record+dataEntryOffset, (char*)data+dataReadOffset, sizeof(int));
    				dataEntryOffset += sizeof(int);

    				//store the byte location of the attribute end
    				memcpy((char*)record+offset, &dataEntryOffset, sizeof(short int));

    				//offset keeps track of the position to insert the attribute end
    				offset += sizeof(short int);
    				dataReadOffset += sizeof(int);
    				break;
    			case 1:
    				//if the attribute type is 'float'
    				//insert the attribute in the internal record
    				memcpy((char*)record+dataEntryOffset, (char*)data+dataReadOffset, sizeof(float));
    				dataEntryOffset += sizeof(float);

    				//store the byte location of the attribute end
    				memcpy((char*)record+offset, &dataEntryOffset, sizeof(short int));

    				//offset keeps track of the position to insert the attribute end
					offset += sizeof(short int);
					dataReadOffset += sizeof(float);
					break;
    			case 2:
    				//if the attribute type is 'varchar'

    				//get the actual length of this variable length attribute
    				int varAttrLength;
    				memcpy(&varAttrLength, (char*)data + dataReadOffset, sizeof(int));
    				dataReadOffset += sizeof(int);

    				//insert the attribute in the internal record
    				memcpy((char*)record+dataEntryOffset, (char*)data+dataReadOffset, varAttrLength);
    				dataEntryOffset += varAttrLength;

    				//offset keeps track of the position to insert the attribute end
    				memcpy((char*)record+offset, &dataEntryOffset, sizeof(short int));
					offset += sizeof(short int);
					dataReadOffset += varAttrLength;
					break;
    		}
    	}else{
    		//if the attribute is null
    		//insert '-1' in the attribute offset
    		short int nullOffset = -1;
    		memcpy((char*)record+offset, &nullOffset, sizeof(short int));
    		offset += sizeof(short int);
    	}
    }

    //get the length of the newly created internal record
    int lengthOfRecord = dataEntryOffset;

    RC success;

    //check if the file is empty
    if(fileHandle.getNumberOfPages() == 0){
    	unsigned char *newPage= (unsigned char*)malloc(PAGE_SIZE);

    	//since the file is empty, this is the first record to be inserted
    	//page numbers start from zero
    	//slot numbers start from one
    	rid.pageNum = 0;
    	rid.slotNum = 1;

    	//append this page to the file
    	success = AppendNewPage(fileHandle, newPage, record, lengthOfRecord);

    	//free the memory
    	free(newPage);
    }
    else{
    	//the file already contains some pages

    	//get the number of pages the file already contains
    	unsigned numberOfPages = fileHandle.getNumberOfPages();
    	short int freeSpace=0, numberOfSlots=0, lastSlotOffset=0;
    	unsigned char *currentPage = (unsigned char*)malloc(PAGE_SIZE);

    	//get the free space in the last page
    	fileHandle.handler->seekg(-sizeof(short int), ios_base::end);
    	fileHandle.handler->read((char*)&freeSpace, sizeof(short int));

    	//if last page has sufficient free space (record length + 2bytes for storing the record offset)
    	if(freeSpace >= (lengthOfRecord + (int)sizeof(short int))){

    		//read the last page
    		success = fileHandle.readPage(numberOfPages -1, currentPage);
    		if(success == 0){

    			//set the page number where record was inserted
    			rid.pageNum = numberOfPages - 1;

    			//update the free space now left in the page after insertion of the record
    			freeSpace -= (lengthOfRecord + sizeof(short int));
    			memcpy((char*)currentPage +(PAGE_SIZE -sizeof(short int)), &freeSpace, sizeof(short int));

    			//get the number of slots already present in the page
    			memcpy(&numberOfSlots, (char*)currentPage +(PAGE_SIZE -2*sizeof(short int)), sizeof(short int));

    			//get the end of the last record present in the page
    			memcpy(&lastSlotOffset, (char*)currentPage + (PAGE_SIZE - (2+numberOfSlots)* sizeof(short int)), sizeof(short int));

    			//insert the record in the page
    			memcpy((char*)currentPage + lastSlotOffset, (char*)record, lengthOfRecord);

    			//update the number of slots and last record's offset
    			numberOfSlots += 1;
    			lastSlotOffset += lengthOfRecord;
    			memcpy((char*)currentPage +(PAGE_SIZE -2*sizeof(short int)), &numberOfSlots, sizeof(short int));
    			memcpy((char*)currentPage + (PAGE_SIZE - (2+numberOfSlots)* sizeof(short int)), &lastSlotOffset, sizeof(short int));

    			rid.slotNum = numberOfSlots;
    			success = success & fileHandle.writePage(rid.pageNum,currentPage);
    		}

    	}else{
    		//the last page doesn't have sufficient space

    		int findFreeSpaceOffset=0;
    		unsigned pageCounter = 0;

    		//find the page which has sufficient space, starting from page number 0
    		while(pageCounter<numberOfPages && freeSpace< (lengthOfRecord+(int)sizeof(short int))){
    			findFreeSpaceOffset += PAGE_SIZE;
    			fileHandle.handler->seekg(findFreeSpaceOffset-sizeof(short int), ios_base::beg);
				fileHandle.handler->read((char*)&freeSpace, sizeof(short int));
				pageCounter++;
    		}

    		//if we found the page with enough space
    		if((pageCounter-1)<numberOfPages && freeSpace >= (lengthOfRecord + (int)sizeof(short int))){
    			//read the said page
    			success = fileHandle.readPage(pageCounter-1, currentPage);
				if(success == 0){
					//set the page number where record was inserted
					rid.pageNum = pageCounter-1;

					//update the free space now left in the page after insertion of the record
					freeSpace -= (lengthOfRecord + sizeof(short int));
					memcpy((char*)currentPage +(PAGE_SIZE -sizeof(short int)), &freeSpace, sizeof(short int));

					//get the number of slots already present in the page
					memcpy(&numberOfSlots, (char*)currentPage +(PAGE_SIZE -2*sizeof(short int)), sizeof(short int));

					//get the end of the last record present in the page
					memcpy(&lastSlotOffset, (char*)currentPage + (PAGE_SIZE - (2+numberOfSlots)* sizeof(short int)), sizeof(short int));

					//insert the record in the page
					memcpy((char*)currentPage + lastSlotOffset, (char*)record, lengthOfRecord);

					//update the number of slots and last record's offset
					numberOfSlots += 1;
					lastSlotOffset += lengthOfRecord;
					memcpy((char*)currentPage +(PAGE_SIZE -2*sizeof(short int)), &numberOfSlots, sizeof(short int));
					memcpy((char*)currentPage + (PAGE_SIZE - (2+numberOfSlots)* sizeof(short int)), &lastSlotOffset, sizeof(short int));

					rid.slotNum = numberOfSlots;
					success = success & fileHandle.writePage(rid.pageNum,currentPage);
				}
    		}else{
    			//none of the existing pages have enough space
    			//apend new page
    			rid.pageNum = fileHandle.getNumberOfPages();
    			rid.slotNum = 1;
    			success = AppendNewPage(fileHandle, currentPage, record, lengthOfRecord);
    		}
    	}
    	//free the memory
    	free(currentPage);
    }
    //free the memory
    free(nullFieldsIndicator);
    free(record);
	return success;
}

/**
 * Reads a record from the page of a file present at the specified page number and slot number
 * @param fileHandle object of FileHandle class, operates on the file
 * @param recordDescriptor works as schema of the record
 * @param rid contains the page number and slot number of the record to be fetched
 * @param data the record to be fetched and returned
 * @return RC whether the operation was successful or not, 0 or 1
 */
RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
    RC success = 0;

    //if the record slot number is 0, return operation failure since slot numbers start from 1
    if(rid.slotNum == 0){
    	success = -1;
    }
    else{

    	//read the page specified
		unsigned char *currentPage = (unsigned char*)malloc(PAGE_SIZE);
		success = success & fileHandle.readPage(rid.pageNum, currentPage);
		if(success == 0){
			int numberOfAttributes = recordDescriptor.size();
			int nullFieldIndicatorByteSize = ceil((double)numberOfAttributes/ CHAR_BIT);

			//initially set the nulls field indicator to 0
			unsigned char *nullFieldsIndicator = (unsigned char *) malloc(nullFieldIndicatorByteSize);
			memset(nullFieldsIndicator, 0, nullFieldIndicatorByteSize);

			//recordBeginOffset stores the offset from where the record starts in the page
			//recordEndOffset stores the offset where the record ends in the page
			short int recordBeginOffset = 0, recordEndOffset = 0, numberOfSlots=0;
			memcpy(&recordEndOffset, (char*)currentPage+ (PAGE_SIZE - (2+rid.slotNum)* sizeof(short int)), sizeof(short int));
			memcpy(&numberOfSlots, (char*)currentPage + (PAGE_SIZE - (2* sizeof(short int))), sizeof(short int));

			//if the first record is asked for, recordBeginOffset will be 0, otherwise fetch the recordBeginOffset
			if(rid.slotNum > 1){
				memcpy(&recordBeginOffset, (char*)currentPage+ (PAGE_SIZE - (2+rid.slotNum -1)* sizeof(short int)), sizeof(short int));
			}else{
				recordBeginOffset = 0;
			}

			//get the internal record
			void *record = malloc(1000);
			memcpy((char*)record, (char*)currentPage + recordBeginOffset, recordEndOffset - recordBeginOffset);

			//this is the record to be returned
			//copy the nullFieldsIndicator to the record
			memcpy((char*)data, nullFieldsIndicator, nullFieldIndicatorByteSize);

			//attribEndOffset stores the position where the attribute ends in the record
			//readOffset stores the position from where attribute locations are read
			//dataReadoffset stores the position from where the actual attribute value is fetched
			//writeOffset stores the current position to write in the 'data' record
			int attribEndOffset=0, readOffset=0, writeOffset=nullFieldIndicatorByteSize;
			int dataReadOffset = numberOfAttributes * sizeof(short int);

			bool foundNulls = false;
			for(int i =0; i< numberOfAttributes; i++){
				memcpy(&attribEndOffset,(char*)record + readOffset , sizeof(short int));

				//if the attribute end offset is found to be -1, the attribute is null
				if(attribEndOffset == 0xffff){
					//update the nullFieldsIndicator
					nullFieldsIndicator[i/CHAR_BIT] = nullFieldsIndicator[i/CHAR_BIT] | (1 << (CHAR_BIT * nullFieldIndicatorByteSize - 1 - i));
					foundNulls = true;
				}
				else{
					switch(recordDescriptor[i].type){
						case 0:
							//if the attribute type was 'int'
							memcpy((char*)data + writeOffset, (char*)record+dataReadOffset, sizeof(int));
							writeOffset += sizeof(int);
							break;
						case 1:
							//if the attribute type was 'float'
							memcpy((char*)data + writeOffset, (char*)record+dataReadOffset, sizeof(float));
							writeOffset += sizeof(float);
							break;
						case 2:
							//if the attribute type was 'varchar'
							//get the actual length of the variable attribute
							int varAttrLength = attribEndOffset - dataReadOffset;

							//write the length of the attribute to the record 'data'
							memcpy((char*)data+writeOffset, &varAttrLength, sizeof(int));
							writeOffset += sizeof(int);

							//write the value of the attribute to 'data'
							memcpy((char*)data + writeOffset, (char*)record + dataReadOffset, varAttrLength);
							writeOffset += varAttrLength;
							break;
					}
					dataReadOffset = attribEndOffset;
				}
				readOffset += sizeof(short int);
			}
			if(foundNulls){
				//if there were any null attributes, update the nullFieldIndicator in the 'data'
				memcpy((char*)data, nullFieldsIndicator, nullFieldIndicatorByteSize);
			}
			//free the memory
			free(nullFieldsIndicator);
			free(record);
		}
    }
    return success;
}

/**
 * Prints the given record
 * @param recordDescriptor works as schema of the record
 * @param data the record to be printed
 * @return RC whether the operation was successful or not, 0 or 1
 */
RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {
	int numberOfAttributes = recordDescriptor.size();
	int nullFieldIndicatorByteSize = ceil((double)numberOfAttributes/ CHAR_BIT);

	//get the nullFieldsIndicator
	unsigned char *nullFieldsIndicator = (unsigned char *) malloc(nullFieldIndicatorByteSize);
	memcpy(nullFieldsIndicator, (char*)data, nullFieldIndicatorByteSize);

	//offset stores the current position to be read from
	int offset = nullFieldIndicatorByteSize;

	for(int i=0; i< numberOfAttributes; i++){

			//check if the attribute is null
	    	if(!(nullFieldsIndicator[i/CHAR_BIT] & (1 << (CHAR_BIT * nullFieldIndicatorByteSize - 1 - i)))){
	    		switch(recordDescriptor[i].type){
	    			case 0:
	    				//if the attribute type is 'int'
	    				cout<<recordDescriptor[i].name+ ": ";
	    				int dataI;
	    				memcpy(&dataI, (char*)data+offset, sizeof(int));
	    				cout<<dataI<<endl;
	    				offset += sizeof(int);
	    				break;
	    			case 1:
	    				//if the attribute type is 'float'
	    				cout<<recordDescriptor[i].name+ ": ";
						float dataF;
						memcpy(&dataF, (char*)data+offset, sizeof(float));
						cout<<dataF<<endl;
						offset += sizeof(float);
						break;
	    			case 2:
	    				//if the attribute type is 'varchar'
	    				int varAttrLength;
	    				cout<<recordDescriptor[i].name+ ": ";
	    				char *dataV = (char *)malloc(recordDescriptor[i].length);
	    				memcpy(&varAttrLength, (char*)data + offset, sizeof(int));
	    				offset += sizeof(int);
	    				memcpy(dataV, (char*)data+offset, varAttrLength);
	    				string str(dataV, varAttrLength);
	    				cout<<str<<endl;
						offset += varAttrLength;
						break;
	    		}
	    	}
	    	else{
	    		//the attribute is null, print NULL
					cout<<recordDescriptor[i].name+ ": NULL"<<endl;
			}
	    }
	return 0;
}
