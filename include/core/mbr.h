#ifndef MBR_H
#define MBR_H

#include <cstring>
#include <fstream>
#include <iostream>
#include "system.h"
#include "data_impl.h"
#include "./../feature/fat/bs_system.h"
#include "./../feature/inode/inode_system.h"

const int BOOT = 0x0000;
const int MBR_SIZE = 512;
const int DATA_SIGNATURE = 0x01B8;
// const int NULL = 0x001BC;


/* const char BOOT = 0x0000;
const int MBR_SIZE = 512;
const short DATA_SIGNATURE = 0x01B8; */
const short NULL_ = 0x001BC;
const int PARTITION_TABLE = 0x01BE;
const int BOOT_SIGNATURE = 0x01FE;
const int BOOT_SECTOR = 0x01FF;


const unsigned int maxSectors = 63;
const unsigned int maxHeads = 256;
const unsigned int maxCylinders = 1024;

enum SpeicherSystem : unsigned char {
BS_FAT,
INODE
};

struct CHS {
    unsigned int c : 10;
    unsigned int h : 8;
    unsigned int s : 6;
};

struct Partition {
    char isBootable;
    SpeicherSystem type;
    //unsigned int length;
    unsigned int sectorsCount;
    CHS* firstSektor;
    CHS* lastSektor;
    CHS* startPtr;
    System* system;
};

class MBR{
public:

MBR(unsigned long MaxSpeicherplatz = 8455716863){
    if(MaxSpeicherplatz > 8455716863){throw std::out_of_range("Der gewünschte Speicherplatz liegt über dem Maximalwert der MBR von 8455716863 Bytes");}
    else {
        this->MaxSpeicherplatz = MaxSpeicherplatz;
    }
}
Partition createPartition(unsigned int Speicherplatz ,SpeicherSystem System = BS_FAT, unsigned int BlockSize = 512){
//Todo check Rest Speicherplatz
Partition Eintrag = {};
Data *dataHandler = new Data_Impl(BlockSize);
Eintrag.type = System;
Eintrag.startPtr = createstartSector();//Todo start ptr zeigen
Eintrag.firstSektor = createSector( getStart()); //Todo First Sektor ermitteln
Eintrag.lastSektor = createSector( getStart() + Speicherplatz );
Eintrag.system = createSystem(System,Speicherplatz,dataHandler);
    Eintrag.isBootable = checkbootable(Eintrag);
    Eintrag.sectorsCount = Speicherplatz /BlockSize;
    Partitions[ParitionEntries] = Eintrag;
    ParitionEntries++; //TODO entfernen
    return Eintrag;
}
unsigned char checkbootable(Partition E){
//Todo check if bootable
    if(E.firstSektor != NULL && E.lastSektor != NULL) {
        return 0x80;
    }else{return 0;}
}
System*  createSystem(SpeicherSystem System, unsigned int Speicher,Data* datahandler,unsigned int BlockSize = 512){
    void* memory = ::operator new(Speicher);
if(System == BS_FAT){return bootBSFat(memory ,BlockSize,Speicher,datahandler);}
else if(System = INODE){return bootINode(memory,BlockSize,Speicher,datahandler);}
else{throw("Das System konnte nicht erstellt werden");}
}


unsigned long getStart() {
    if(Partitions[0].startPtr == NULL) {
        return 0;
    }
    else {
        return checkLBA();
    }
}
unsigned long checkLBA() {
    for(int i = 0; i < 4; i++) {
        if(Partitions[i].lastSektor == NULL) {
            int c = Partitions[i - 1].lastSektor->c;
            int h = Partitions[i - 1].lastSektor->h;
            int s = Partitions[i - 1].lastSektor->s ;
            unsigned long Ergebnis = 512 * ((c * maxHeads + h) * maxSectors + ( s - 1 ));
            return Ergebnis;
        }
    } return 0;
}


    CHS* createSector(unsigned long speicherplatzInBytes, unsigned int BlockSize = 512) {
    CHS* sector = new CHS ;
    unsigned int Speicherplatz = speicherplatzInBytes / BlockSize;
    unsigned long maxSpeicher =  maxCylinders * maxHeads * maxSectors; //Todo Restspeicher abziehen
    if ( Speicherplatz >= maxSpeicher) {
        throw std::out_of_range("Der gewünschte Speicherplatz liegt über dem Maximalwert der MBR von 8455716863 Bytes");
    }


    // Direkte Berechnung der CHS-Werte
    sector->s = ( Speicherplatz % maxSectors) + 1; // Sektoren starten bei 1
        Speicherplatz /= maxSectors;
    sector->h =  Speicherplatz % maxHeads;
     Speicherplatz /= maxHeads;
    sector->c =  Speicherplatz;

    return sector;

}

CHS* createstartSector() { //TODO ist richtig?
        if(Partitions[0].lastSektor == NULL && Partitions[0].firstSektor == NULL) {
           return  createSector(0);
        }
        else{
        return  createSector(getStart());
        }
        }


bool boot(int bIndex = 0){
if(Partitions[bIndex].isBootable != 0){
//return Partitions[bIndex].isBootable != 1 && Partitions[bIndex].system->boot();
    return 0;
}
return 1;}

unsigned long checkSizeReserviert() {
        unsigned long MemoryAllocated = checkLBA();
        std::cout << "Es  ist " << MemoryAllocated  << " Byte groß" << std::endl;
        return MemoryAllocated;;
    }

    int checkPartitionsize(Partition p) {
        int c = p.lastSektor->c;
        int h = p.lastSektor->h;
        int s = p.lastSektor->s ;
        unsigned long Ergebnis = 512 * ((c * maxHeads + h) * maxSectors + ( s - 1 ));
        return Ergebnis;
    }


unsigned int getMaxSpeicherplatz(){return this->MaxSpeicherplatz;}
Partition* getPartition(){ return Partitions;}

Partition getSingularPartition(int i = 0) {
        if(i > 3) {
            throw std::out_of_range("Partition index übersteigt den Maximalenwert von 4");
        }
        return Partitions[i];
    }

    BsFat* bootBSFat(void* memory,unsigned int blockSize,unsigned int memorySize, Data *dataHandler){
        BsFat* B = BsFat::create(memory, memorySize,blockSize, dataHandler);
        return B;
        }
    INodeSystem* bootINode(void* memory,unsigned int blockSize,unsigned int memorySize, Data *dataHandler){
        INodeSystem* I = INodeSystem::create(memory, memorySize,blockSize, dataHandler);
        return I;
    }
private:
    Partition Partitions[4] = { 0 };
    const int identificationCode = 0xAA55;
    unsigned long MaxSpeicherplatz;
    unsigned int ParitionEntries = 0;

};




#endif //MBR_H

