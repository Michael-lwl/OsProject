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

struct partition {
  char isBootable;
  CHS *firstSektor;
  CHS *lastSektor;
  CHS *startPtr;
  unsigned int length;
  char type;
};

class Master_Boot_Record {
public:
  Master_Boot_Record() {

    // Todo Inputs für memorySize und BlockSize für die händische Abfrage
    // einfügen
    bool boot() { return 1; }
  }
  BsFat *bootBSFat(unsigned int blockSize, unsigned int memorySize,
                   Data *dataHandler) {
    BsFat *B = BsFat::create(memorySize, blockSize, dataHandler);
    return B;
  }

  //   INode* bootINode(int blockSize, int memorySize, Data *dataHandler){
  //       INode* I = INode::create(memorySize,blockSize, dataHandler);
  //       return I;
  //     }

  // Getter
  unsigned int getSectorsCount() { return sectorsCount; }
  unsigned int getDiskSignature() { return diskSignature; }
  // Setter

  void setSectorcount(unsigned int sectorsCount) {
    this->sectorsCount = sectorsCount;
  }
  void setDiskSignature(unsigned int diskSignature) {
    this->diskSignature = diskSignature;
  }

private:
  partition Partitions[4];
  unsigned int sectorsCount;
  unsigned int diskSignature; // normalerweise nur in Windows
  const int identificationCode = 0xAA55;
};

#endif // MBR_H
