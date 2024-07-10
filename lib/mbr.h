#ifndef MBR_H
#define MBR_H
#include "utils.h"
#include <iostream>
#include <fstream>
#include <cstring>

const int BOOT= 0x0000;
const int MBR_SIZE = 512;
const int DATA_SIGNATURE = 0x01B8;
const int NULL = 0x001BC;
const int PARTITION_TABLE = 0x01BE;
const int BOOT_SIGNATURE = 0x01FE;
const int BOOT_SECTOR = 0x01FF;

class Partition {
    public:
    Partition(unsigned char bootable,
              unsigned int partitionType,
              unsigned int startSector,
              unsigned int firstSector,
              unsigned int lastSector,
              unsigned int sectorsCount,
              unsigned int diskSignature);
        //Setter
    void setBootable(unsigned char bootable){
        bootable = bootable;
    }
    void setPartitionType(unsigned int partitionType){
        partitionType = partitionType;
    }
    void setStartSector(unsigned int startSector){
        startSector = startSector;
    }
    void setFirstSector(unsigned int firstSector){
        firstSector = firstSector;
    }
    void setLastSector(unsigned int lastSector){
        lastSector = lastSector;
    }
    void setSectorcount(unsigned int sectorsCount){
        sectorsCount = sectorsCount;
    }
    void setDiskSignature(unsigned int diskSignature){
        diskSignature = diskSignature;
    }

    //Getter
    unsigned char getBootable(){
    return bootable;
    }
    unsigned int getPartitionType(){
    return partition Type;
    }
    unsigned int getStartSector(){
    return startSector;
    }
    unsigned int getFirstSector(){
    return firstSector;
    }
    unsigned int getLastSector(){
    return lastSector;
    }
    unsigned int getSectorsCount(){
    return sectorsCount;
    }
    unsigned int getDiskSignature(){
    return diskSignature;
    }


    private:
        unsigned char bootable; //Status ob bootable oder nicht
        unsigned int partitionType;
        unsigned int startSector; // per LBA Methode
        unsigned int firstSector;
        unsigned int lastSector;
        unsigned int sectorsCount;
        unsigned int diskSignature; //normalerweise nur in Windows
};



#endif //MBR_H
