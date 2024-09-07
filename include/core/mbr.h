#ifndef MBR_H
#define MBR_H
#include "./../array.h"
#include "./../feature/fat/bs_system.h"
#include "./../feature/inode/inode_system.h"
#include "./data.h"
#include "./data_impl.h"
#include "./system.h"
#include <cstring>
#include <fstream>
#include <iostream>

const int BOOT = 0x0000;
const int MBR_SIZE = 512;
const int DATA_SIGNATURE = 0x01B8;
// const int NULL = 0x001BC;
const int PARTITION_TABLE = 0x01BE;
const int BOOT_SIGNATURE = 0x01FE;
const int BOOT_SECTOR = 0x01FF;

struct CHS {
  unsigned char c;
  unsigned char h;
  unsigned char s;
};

struct Partition {
  char isBootable;
  CHS *firstSektor;
  CHS *lastSektor;
  CHS *startPtr;
  unsigned int length;
  char type;
  System* system;
};

enum SpeicherSystem {
    BS_FAT,
    INODE
};


class MBR {
public:
    static const size_t MAX_PARTITION_COUNT = 4;

  MBR(unsigned long long driveSize) {

    // Todo Inputs für memorySize und BlockSize für die händische Abfrage
    // einfügen
  }
  BsFat *bootBSFat(unsigned int blockSize, unsigned int memorySize, Data *dataHandler) {
    void *memory = ::operator new(memorySize); // Raw allocation
    BsFat *B = BsFat::create(memory, memorySize, blockSize, dataHandler);
    return B;
  }

   INodeSystem* bootINode(int blockSize, int memorySize, Data *dataHandler){
    void *memory = ::operator new(memorySize); // Raw allocation
    INodeSystem* I = INodeSystem::create(memory, memorySize,blockSize, dataHandler);
    return I;
   }

   Partition* createPartition(unsigned long long memorySize, BlockSizes blockSize = BlockSizes::KIB_4,SpeicherSystem ss = SpeicherSystem::BS_FAT) {return nullptr;}

  // Getter
  unsigned int getSectorsCount() { return sectorsCount; }
  unsigned int getDiskSignature() { return diskSignature; }
  Partition* getPartitions() {return partitions;}

  size_t getPartitionCount() {
      size_t output = 0;
      for (size_t i = 0; i < MAX_PARTITION_COUNT; i++) {
          if (partitions[i].firstSektor == nullptr || partitions[i].length != 0)
              output++;
      }
      return output;
  }
  // Setter

  void setSectorcount(unsigned int sectorsCount) {
    this->sectorsCount = sectorsCount;
  }
  void setDiskSignature(unsigned int diskSignature) {
    this->diskSignature = diskSignature;
  }

private:
  Partition partitions[MAX_PARTITION_COUNT];
  unsigned int sectorsCount;
  unsigned int diskSignature; // normalerweise nur in Windows
  const int identificationCode = 0xAA55;
};

#endif // MBR_H
