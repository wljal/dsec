#include "dataStructs.h"

#include <cstdint>
#include <cstdio>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <iterator>
#include <winbase.h>
#include <windows.h>
#include <winnt.h>

class parser {
	HANDLE drive;
	NonResidentAttributeHeader *mftFileAttribute;

	BootSector mftBootSec;

	int err(const char* error, bool getLastError) {
		if(getLastError)
			printf("Code: %ld\n", GetLastError());
		perror(error);
		return -1;
	}

	public : int cleanup() {
		if(CloseHandle(drive) == 0) return err("Failed to closehandle of drive", true);
		return 0;
	}

	int read(void* buffer, uint64_t from, uint64_t count) {
		DWORD bytesAC;
		LONG x32high = from >> 32;

		SetFilePointer(drive, from & 0xFFFFFFFF, &x32high, FILE_BEGIN);
		ReadFile(drive, buffer, count, &bytesAC, NULL);

		if(bytesAC != count) return err("Bytes accessed doesnt equal the bytes requested", false);
		return 0;
	}

	public : int init() {
		drive = CreateFile("\\\\.\\C:", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		
		if(read(&mftBootSec, 0, 512) != 0) return -1;

		//Read first KB of mft
		uint8_t mftFile[MFTFILESIZE];
		uint64_t bPc = mftBootSec.sectorsPerCluster * mftBootSec.bytesPerSector;
		if(read(&mftFile, mftBootSec.mftStart * bPc, MFTFILESIZE) != 0) return -1;

		//Cast to FileRecordHeader
		FileRecordHeader *mftFileRecord = (FileRecordHeader*) mftFile;
		AttributeHeader *mftAttribute = (AttributeHeader*) mftFile + mftFileRecord->firstAttributeOffset;

		NonResidentAttributeHeader *dataAttribute = nullptr;
		if(mftFileRecord->magic != 0x454C4946) return err("mftFileRecord's magic is not correct", false);

		while(true) {
			if(mftAttribute->attributeType == 0x80) dataAttribute = (NonResidentAttributeHeader *) mftAttribute;
			else if(mftAttribute->attributeType == 0xFFFFFFFF) break;

			mftAttribute = (AttributeHeader*) ((uint8_t) mftAttribute + mftAttribute->length); 
		}
		
		if(dataAttribute == nullptr) return err("Failed to find MFT data attribute", false);
		this->mftFileAttribute = dataAttribute;
		return 0;
	}

	void parseMFTDataRuns() {
		FileNameAttributeHeader *fileNameAttribute = (FileNameAttributeHeader *) mftFileAttribute;

		if(fileNameAttribute->namespaceType != 2 && !fileNameAttribute->nonResident) {

		}

	}
};
