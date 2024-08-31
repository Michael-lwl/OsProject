#ifndef MBR_H
#define MBR_H
#include <iostream>
#include <fstream>
#include <cstring>
#include "../include/core/system.h"
#include "../include/core/data_impl.h"
#include "../include/core/data.h"
#include "../include/array.h"
#include "../include/feature/fat/bs_system.h"
#include "../include/feature/inode/inode_system.h"

const int BOOT = 0x0000;
const int MBR_SIZE = 512;
const int DATA_SIGNATURE = 0x01B8;
//const int NULL = 0x001BC;
const int PARTITION_TABLE = 0x01BE;
const int BOOT_SIGNATURE = 0x01FE;
const int BOOT_SECTOR = 0x01FF;

const int maxSectors = 63;
const int maxHeads = 256;
const int maxCylinders = 1024;
const int blockSize = 512;

struct CHS {
    unsigned int c : 10;
    unsigned char h : 6;
    unsigned char s;
};

struct partition {
   char isBootable;
   char type;
   //unsigned int length;
  unsigned int sectorsCount;
   CHS* firstSektor;
   CHS* lastSektor;
   CHS* startPtr;
   System* system;
};

class Master_Boot_Record{
public:

Master_Boot_Record(unsigned int Speicher, char System){
createPartition(Speicher,1);
}
void createPartition(unsigned int Speicher, char System){
partition Eintrag;
Data *dataHandler = new Data_Impl(blockSize);
Eintrag.isBootable = 1;
Eintrag.type = 1;
Eintrag.firstSektor = createSector(getSectorCount());
Eintrag.lastSektor = createSector(Speicher);
Eintrag.sectorsCount = getSectorCount();
    if(Partitions[0].firstSektor != Partitions[0].startPtr ){
        Eintrag.startPtr = Eintrag.firstSektor;
    }
    else{
    Eintrag.startPtr = Partitions[0].startPtr;
    }
Eintrag.system = createSystem(System,Speicher,dataHandler);
}

System*  createSystem(char System, unsigned int Speicher,Data* datahandler){
if(System == 1){return bootBSFat(blockSize,Speicher,datahandler);}
else if(System = 2){return bootINode(blockSize,Speicher,datahandler);}
else{throw("Das System konnte nicht erstellt werden");}
}

unsigned int getSectorCount(){
int SectorCount = 0;
for(int i = 0; i < 4; i++){
if(Partitions[i].firstSektor->c != 0 || Partitions[i].firstSektor->h != 0 || Partitions[i].firstSektor->s != 0){
SectorCount++;
}
if(Partitions[i].lastSektor->c != 0 || Partitions[i].lastSektor->h != 0 || Partitions[i].lastSektor->s != 0){
SectorCount++;
}

}
return SectorCount;
}

    CHS* createSector(unsigned int block) {
    CHS* sector;

    const int maxSectors = 63;        // Sektoren reichen von 1 bis 63
    const int maxHeads = 256;         // Köpfe reichen von 0 bis 255
    const int maxCylinders = 1024;    // Zylinder reichen von 0 bis 1023

    // Maximale Anzahl an Blöcken im gegebenen Bereich
    const unsigned int maxBlocks = maxCylinders * maxHeads * maxSectors;

    if (block > maxBlocks) {
        throw std::out_of_range("Der gewünschte Index ist außerhalb unseres Speicherplatzes");
    }

    // Direkte Berechnung der CHS-Werte
    sector->s = (block % maxSectors) + 1; // Sektoren starten bei 1
    block /= maxSectors;
    sector->h = block % maxHeads;
    block /= maxHeads;
    sector->c = block;

    return sector;
}


//Todo Inputs für memorySize und BlockSize für die händische Abfrage einfügen
bool boot(int bIndex = 0){
if(Partitions[bIndex].isBootable != 1){
return Partitions[bIndex].isBootable != 1 && Partitions[bIndex].system->boot();
}
return 1;
}

    BsFat* bootBSFat(unsigned int blockSize,unsigned int memorySize, Data *dataHandler){
    BsFat* B = BsFat::create(memorySize,blockSize, dataHandler);
    return B;
    }

      INode* bootINode(int blockSize, int memorySize, Data *dataHandler){
       INode* I = INode::create(memorySize,blockSize, dataHandler);
      return I;
     }


    //Getter
    unsigned int getDiskSignature(){ return diskSignature; }
    //Setter


    void setDiskSignature(unsigned int diskSignature){this->diskSignature = diskSignature;}

private:
    partition Partitions[4];
    unsigned int diskSignature; //normalerweise nur in Windows
    const int identificationCode = 0xAA55;

};


#endif //MBR_H
