#ifndef MBR_H
#define MBR_H

#include <cstring>
#include <fstream>
#include <iostream>
#include "system.h"
#include "data_impl.h"
#include "./../feature/fat/bs_system.h"
#include "./../feature/inode/inode_system.h"

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

MBR(long MaxSpeicherplatz){
this->MaxSpeicherplatz = MaxSpeicherplatz;
}
Partition createPartition(unsigned long long Speicherplatz ,SpeicherSystem System = BS_FAT, unsigned int BlockSize = 512){
//Todo check Rest Speicherplatz
if(Partitions[0].firstSektor != NULL && Partitions[0].lastSektor != NULL) {
    unsigned long long LBA = checkLBA();
    if(MaxSpeicherplatz < Speicherplatz + LBA ) {
        throw std::out_of_range("Der gewünschte Speicherplatz übersteigt den Restspeicher des MBRs. Partition konnte nicht angelegt werden");
    }
}
Partition Eintrag = {};
//Data *dataHandler = new Data_Impl(this->BlockSize);
Eintrag.type = System;
Eintrag.startPtr = createstartSector();//Todo start ptr zeigen
Eintrag.firstSektor = createSector( getStart()); //Todo First Sektor ermitteln
Eintrag.lastSektor = createSector( getStart() + Speicherplatz );
//Eintrag.system = createSystem(System,Speicherplatz,dataHandler);
    Eintrag.isBootable = checkbootable(Eintrag);
    Eintrag.sectorsCount = Speicherplatz /BlockSize;
    for(int i = 0; i < 4; i++) {
        if(Partitions[i].firstSektor == NULL && Partitions[i].lastSektor == NULL) {
            Partitions[i] = Eintrag;
            break;
        }
    }

    return Eintrag;
}
unsigned char checkbootable(Partition E){
//Todo check if bootable
    if(E.firstSektor != NULL && E.lastSektor != NULL) {
        return 0x80;
    }else{return 0;}
}
//System*  createSystem(SpeicherSystem System, unsigned int Speicher,Data* datahandler){
//    void* memory = ::operator new(Speicher);
//(System == BS_FAT){return bootBSFat(memory ,this->BlockSize,Speicher,datahandler);}
//else if(System = INODE){return bootINode(memory,this->BlockSize,Speicher,datahandler);}
//else{throw("Das System konnte nicht erstellt werden");}
//

unsigned long long getStart() {
    if(Partitions[0].startPtr == NULL) {
        return 0;
    }
    else {
        return checkLBA();
    }
}
unsigned long long checkLBA(){
    for(int i = 0; i < 5; i++) {
        if(   i == 4 || Partitions[i].lastSektor == NULL ) {
            long c = Partitions[i - 1].lastSektor->c;
            long h = Partitions[i - 1].lastSektor->h;
            long s = Partitions[i - 1].lastSektor->s ;
            unsigned long long Ergebnis = 512 * ((c * maxHeads + h) * maxSectors + ( s - 1 ));
            return Ergebnis;
        }
    }
}


    CHS* createSector(unsigned long long speicherplatzInBytes, unsigned int BlockSize = 512) {
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
unsigned long long checkSizeReserviert() {
        unsigned long long MemoryAllocated = checkLBA();
    std::cout << "Es  ist vereits " << MemoryAllocated  << " Byte reserviert" << std::endl;
        return MemoryAllocated;;
    }
    unsigned long long checkPartitionsize(Partition p) {
 unsigned long long c1 = p.firstSektor->c;
 unsigned long long h1 = p.firstSektor->h;
 unsigned long long s1 = p.firstSektor->s ;
 unsigned long long c2 = p.lastSektor->c;
 unsigned long long h2 = p.lastSektor->h;
 unsigned long long s2 = p.lastSektor->s ;
   unsigned long long c = c2 - c1;
   unsigned long long h = h2 - h1;
   unsigned long long s = s2 - s1;
        unsigned long long Ergebnis = 512 * ((c * maxHeads + h) * maxSectors + ( s -1  ));
    std::cout << "Die Partition ist " << Ergebnis  << " Byte groß" << std::endl;
        return Ergebnis;
    }

    void deletePartition(int i) {
    unsigned long long reserved[4] = {0};
    if(Partitions[i].firstSektor != NULL && Partitions[i].lastSektor != NULL && i < 4 && i > 0) {
        throw std::out_of_range("Es gibt diesen Eintrag nicht");
    }
    else {
        Partition p[] = {0};

        for (int j = 0; j < 4 - 1; j++) {

            if(i != j) {
                reserved[j] = checkPartitionsize(getSingularPartition(j ));

            }
            else if (i == j && i < 5) {
                if(Partitions[j + 1].firstSektor != NULL) {
                    reserved[j] = checkPartitionsize(getSingularPartition(j + 1));
                    i++;
                }
            }


        }
        Partitions[0] = {0};
        Partitions[1] = {0};
        Partitions[2] = {0};
        Partitions[3] = {0};
        for(int k = 0; k < 4; k++) {

            Partitions[k] = createPartition( reserved[k]);

        }

    }
}


unsigned long long getMaxSpeicherplatz(){return this->MaxSpeicherplatz;}
Partition* getPartition(){ return Partitions;}
Partition getSingularPartition(int i = 0) {
        if(i > 3) {
            throw std::out_of_range("Partition index übersteigt den Maximalenwert von 4");
        }
        return Partitions[i];
    }
    System*  createSystem(SpeicherSystem System, unsigned int Speicher,Data* datahandler,unsigned int BlockSize = 512){
        void* memory = ::operator new(Speicher);
        if(System == BS_FAT){return bootBSFat(memory ,BlockSize,Speicher,datahandler);}
        else if(System == INODE){return bootINode(memory,BlockSize,Speicher,datahandler);}
        else{throw("Das System konnte nicht erstellt werden");}
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

