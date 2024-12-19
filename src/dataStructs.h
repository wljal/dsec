#pragma once

#include <assert.h>
#include <stdint.h>
#include <windows.h>

#define MFTFILESIZE (1024)

#pragma pack(push,1)
struct BootSector {
    uint8_t     jump[3]; 
    char        name[8];
    uint16_t    bytesPerSector;         // The number of bytes in a sector. This should be 512.
    uint8_t     sectorsPerCluster;      // The number of sectors in a cluster. Clusters are used for less-granular access to the partition. They're usually 4KB.
    uint16_t    reservedSectors;
    uint8_t     unused0[3];
    uint16_t    unused1;
    uint8_t     media;
    uint16_t    unused2;
    uint16_t    sectorsPerTrack;
    uint16_t    headsPerCylinder;
    uint32_t    hiddenSectors;
    uint32_t    unused3;
    uint32_t    unused4;
    uint64_t    totalSectors;
    uint64_t    mftStart;           // The start of the MFT, given as a cluster index.
    uint64_t    mftMirrorStart;
    uint32_t    clustersPerFileRecord;
    uint32_t    clustersPerIndexBlock;
    uint64_t    serialNumber;
    uint32_t    checksum;
    uint8_t     bootloader[426];
    uint16_t    bootSignature;
};
struct FileRecordHeader {
    uint32_t    magic;              // "FILE"
    uint16_t    updateSequenceOffset;
    uint16_t    updateSequenceSize;
    uint64_t    logSequence;
    uint16_t    sequenceNumber;
    uint16_t    hardLinkCount;
    uint16_t    firstAttributeOffset;       // Number of bytes between the start of the header and the first attribute header.
    uint16_t    inUse : 1;          // Whether the record is in use.
    uint16_t    isDirectory : 1;
    uint32_t    usedSize;
    uint32_t    allocatedSize;
    uint64_t    fileReference;
    uint16_t    nextAttributeID;
    uint16_t    unused;
    uint32_t    recordNumber;           // The record number. We'll need this later.
};

struct AttributeHeader {
    uint32_t    attributeType;          // The type of attribute. We'll be interested in $DATA (0x80), and $FILE_NAME (0x30).
    uint32_t    length;             // The length of the attribute in the file record.
    uint8_t     nonResident;            // false = attribute's contents is stored within the file record in the MFT; true = it's stored elsewhere
    uint8_t     nameLength;
    uint16_t    nameOffset;
    uint16_t    flags;
    uint16_t    attributeID;
};

struct ResidentAttributeHeader : AttributeHeader {
    uint32_t    attributeLength;
    uint16_t    attributeOffset;
    uint8_t     indexed;
    uint8_t     unused;
};

struct NonResidentAttributeHeader : AttributeHeader {
    uint64_t    firstCluster;
    uint64_t    lastCluster;
    uint16_t    dataRunsOffset;         // The offset in bytes from the start of attribute header to the description of where the attribute's contents can be found.
    uint16_t    compressionUnit;
    uint32_t    unused;
    uint64_t    attributeAllocated;
    uint64_t    attributeSize;
    uint64_t    streamDataSize;
};
struct FileNameAttributeHeader : NonResidentAttributeHeader {
    uint64_t    parentRecordNumber : 48;    // The record number (see FileRecordHeader.recordNumber) of the directory containing this file.
    uint64_t    sequenceNumber : 16;
    uint64_t    creationTime;
    uint64_t    modificationTime;
    uint64_t    metadataModificationTime;
    uint64_t    readTime;
    uint64_t    allocatedSize;
    uint64_t    realSize;
    uint32_t    flags;
    uint32_t    repase;
    uint8_t     fileNameLength;         // The length of the filename in wide chars.
    uint8_t     namespaceType;          // If this = 2 then it's the DOS name of the file. We ignore these.
    wchar_t     fileName[1];            // The filename in UTF-16.
};
struct File {
    uint64_t    parent;             // The index in the files array of the parent.
    char           *name;               // The file name. Zero-terminated; UTF-8.
};
#pragma pack(pop)
