#ifndef MBR_H
#define MBR_H
#include <iostream>
#include <fstream>
#include <cstring>
#include "system.h"
#include "data_impl.h"
#include "data.h"
#include "../array.h"
#include "../feature/fat/bs_system.h"
#include "../feature/inode/inode_system.h"

const int BOOT= 0x0000;
const int MBR_SIZE = 512;
const int DATA_SIGNATURE = 0x01B8;
//const int NULL = 0x001BC;
const int PARTITION_TABLE = 0x01BE;
const int BOOT_SIGNATURE = 0x01FE;
const int BOOT_SECTOR = 0x01FF;

struct CHS {
    unsigned char c;
    unsigned char h;
    unsigned char s;
};

struct partition {
 //   char isBootable;
    CHS* firstSektor;
    CHS* lastSektor;
    CHS* startPtr;
 //   unsigned int length;
 //   char type;
};

class Master_Boot_Record{
public:

Master_Boot_Record(){
this->bootable = 1;
this->partitionType = 1;
}
    //Todo Inputs für memorySize und BlockSize für die händische Abfrage einfügen
System* boot(unsigned int  memorySize ,unsigned int blockSize){
        if ( this->bootable == NULL || this->bootable == 0 ) {
            throw("Error while booting the file system" );
        }
        else {
            Data *dataHandler = new Data_Impl(blockSize);
            switch(this->partitionType){
                case 1:
                  return bootBSFat(blockSize, memorySize, dataHandler);
                //Case Inode und bs_fat
                case 2:
                  return bootINode(blockSize,memorySize, dataHandler);
                default:
               throw("Error while booting the file system" );
            }

        }

    }
    BsFat* bootBSFat(unsigned int blockSize,unsigned int memorySize, Data *dataHandler){
    BsFat* B = BsFat::create(memorySize,blockSize, dataHandler);
    return B;
    }

//   INode* bootINode(int blockSize, int memorySize, Data *dataHandler){
//       INode* I = INode::create(memorySize,blockSize, dataHandler);
//       return I;
//     }


    //Getter
    unsigned char getBootable(){ return bootable; }
    unsigned int getPartitionType(){ return partitionType;}
    unsigned int getSectorsCount(){ return sectorsCount; }
    unsigned int getDiskSignature(){ return diskSignature; }
    //Setter
    void setBootable(unsigned char bootable){ this->bootable = bootable; }
    void setPartitionType(unsigned int partitionType){ this->partitionType = partitionType;}
    void setSectorcount(unsigned int sectorsCount){ this->sectorsCount = sectorsCount; }
    void setDiskSignature(unsigned int diskSignature){this->diskSignature = diskSignature;}

private:
partition Partitions[4];
    unsigned char bootable; //Status ob bootable oder nicht
    unsigned int partitionType;
    unsigned int sectorsCount;
    unsigned int diskSignature; //normalerweise nur in Windows
    const int identificationCode = 0xAA55;

};
class Partition {
    public:
    Partition(int bootable,
              CHS startSector,
              CHS firstSector,
              CHS lastSector){
              this->startSector = &startSector;
              this->firstSector = &firstSector;
              this->lastSector  = &lastSector;};


    void setStartSector(CHS startSector){this->startSector = &startSector; }
    void setFirstSector(CHS firstSector){this->firstSector = &firstSector;  }
    void setLastSector(CHS lastSector){this->lastSector = &lastSector; }

    CHS getStartSector(){return *startSector; }
    CHS getFirstSector(){ return *firstSector;  }
    CHS getLastSector(){ return *lastSector;}

    private:

       char isBootable;
       unsigned int length;
       char type;
       CHS* startSector; // per LBA Methode
       CHS* firstSector;
       CHS* lastSector;

};

#endif //MBR_H
