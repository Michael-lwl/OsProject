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
#include <iostream>
#include <fstream>
#include <cstring>
#include "./system.h"
#include "./data_impl.h"
#include "./data.h"
#include "../array.h"
#include "../feature/fat/bs_system.h"
#include "../feature/inode/inode_system.h"

/* const char BOOT = 0x0000;
const int MBR_SIZE = 512;
const short DATA_SIGNATURE = 0x01B8; */
const short NULL_ = 0x001BC;
const int PARTITION_TABLE = 0x01BE;
const int BOOT_SIGNATURE = 0x01FE;
const int BOOT_SECTOR = 0x01FF;


const int maxSectors = 63;
const int maxHeads = 256;
const int maxCylinders = 1024;

enum SpeicherSystem : unsigned char {
BS_FAT,
INODE
};

struct CHS {
    unsigned int c : 10;
    unsigned char h : 6;
    unsigned char s;
};

struct partition {
    char isBootable;
    SpeicherSystem type;
    //unsigned int length;
    unsigned int sectorsCount;
    CHS* firstSektor;
    CHS* lastSektor;
    CHS* startPtr;
    System* system;
};

class Master_Boot_Record{
public:

Master_Boot_Record(unsigned int Blöcke, int blocksize){
this->Blöcke = Blöcke;
this->BlockSize = blocksize;

}
void createPartition( SpeicherSystem System){

partition Eintrag;
Data *dataHandler = new Data_Impl(this->BlockSize);
Eintrag.type = System;
Eintrag.firstSektor = createSector(getSectorCount(Eintrag)); //Todo First Sektor ermitteln
Eintrag.lastSektor = createSector(getSectorCount(Eintrag) + getBlöcke());
Eintrag.sectorsCount = getSectorCount(Eintrag);
    if(Partitions[0].firstSektor != Partitions[0].startPtr ){
        Eintrag.startPtr = Eintrag.firstSektor;
        std::cout << "Der Startpointer wurde zum ersten Mal erfolgereich gesetzt" << std::endl;
    }
    else{
    Eintrag.startPtr = Partitions[0].startPtr;
        std::cout << "Der Startpointer wurde erfolgereich gesetzt" << std::endl;
    }
Eintrag.system = createSystem(System,getBlöcke(),dataHandler); //Todo Speichergröße annehmenb
    Eintrag.isBootable = checkbootable(Eintrag);
//
for(int i = 0; i < 4; i++){
if(Partitions[i].firstSektor == NULL || Partitions[i].lastSektor == NULL){
 Partitions[i] = Eintrag;
    std::cout << "Die Partition wurde erfolgreich an der Stelle" << i  <<"weggeschrieben" << std::endl;
 break;
 }
}
}
unsigned char checkbootable(partition E){
//Todo check if bootable
    if(E.firstSektor != NULL || E.lastSektor != NULL) {
        return 0x80;
    }else{return 0;}
}
System*  createSystem(SpeicherSystem System, unsigned int Speicher,Data* datahandler){
    void* memory = ::operator new(Speicher);
if(System == BS_FAT){return bootBSFat(memory ,this->BlockSize,Speicher,datahandler);}
//else if(System = INODE){return bootINode(memory,this->BlockSize,Speicher,datahandler);}
else{throw("Das System konnte nicht erstellt werden");}
}

unsigned int getSectorCount(partition E){
    if(E.firstSektor != NULL || E.lastSektor != NULL) { return (E.lastSektor->c * maxHeads + E.lastSektor->h) * maxSectors + ( E.lastSektor->s - 1); }
    else { return 0; }
};


    CHS* createSector(unsigned int block) {
    CHS* sector = {};
    const unsigned int maxBlocks = maxCylinders * maxHeads * maxSectors;

    if (block >= maxBlocks) {
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


bool boot(int bIndex = 0){
if(Partitions[bIndex].isBootable != 0){
return Partitions[bIndex].isBootable != 1 && Partitions[bIndex].system->boot();
}
return 1;}


    BsFat* bootBSFat(void* memory,unsigned int blockSize,unsigned int memorySize, Data *dataHandler){
    BsFat* B = BsFat::create(memory, memorySize,blockSize, dataHandler);
    return B;
    }




  INodeSystem* bootINode(void* memory,int blockSize, int memorySize, Data *dataHandler){
   INodeSystem* I = INodeSystem::create(memorySize,blockSize, dataHandler);
  return I;
 }

unsigned int getBlöcke(){return this->Blöcke;}
unsigned int getBlocksize(){return this->BlockSize;}


private:
    partition Partitions[4];
    const int identificationCode = 0xAA55;
    unsigned int Blöcke;
    unsigned int BlockSize;

};


#endif //MBR_H

