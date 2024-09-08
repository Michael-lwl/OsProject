#ifndef MBR_H
#define MBR_H

#include "./../feature/fat/bs_system.h"
#include "./../feature/inode/inode_system.h"
#include "data_impl.h"
#include "data_sizes.h"
#include "system.h"
#include <cstring>
#include <fstream>
#include <iostream>

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
  INODE_SYSTEM
};

struct CHS {
  unsigned int c : 10;
  unsigned int h : 8;
  unsigned int s : 6;
};

struct Partition {
  char isBootable;
  SpeicherSystem type;
  // unsigned int length;
  unsigned int sectorsCount;
  CHS *firstSektor;
  CHS *lastSektor;
  CHS *startPtr;
  System* system;
};

class MBR {
public:
  static const size_t MAX_PARTITION_COUNT = 4;

  MBR(unsigned long long driveSize) {

    this->MaxSpeicherplatz = driveSize;
    this->sectorsCount = 0;
    for (size_t i = 0; i < MAX_PARTITION_COUNT; i++) {
        partitions[i] = {
            0,
            SpeicherSystem::BS_FAT,
            0,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        };
    }
  }

  // Getter




  // Getter
  unsigned int getSectorsCount() { return sectorsCount; }
  unsigned int getDiskSignature() { return diskSignature; }
  Partition *getpartitions() { return partitions; }

  size_t getPartitionCount() {
    size_t output = 0;
    for (size_t i = 0; i < MAX_PARTITION_COUNT; i++) {
      if (partitions[i].firstSektor == nullptr)
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


  Partition* createPartition(unsigned long long Speicherplatz, SpeicherSystem System = BS_FAT, BlockSizes BlockSize = BlockSizes::B_512) { // erstellt Partitio
    if (partitions[0].firstSektor != NULL && partitions[0].lastSektor != NULL) {
      unsigned long long LBA = checkLBA();
      if (MaxSpeicherplatz < Speicherplatz + LBA) {
        std::cerr << "Partition index übersteigt den Maximalenwert von 4" << std::endl;
        //Todo was kann ich hier machen wenn kein throw? ich will aus der FUnktion raus
        return nullptr;
      }
    }
    unsigned int sectorsCount = Speicherplatz / BlockSize;
    Data* datahandler = new Data_Impl(BlockSize);
    Partition Eintrag = {
        0,
        System,
        sectorsCount,
        createSector(getStart()),
        createSector(getStart() + Speicherplatz),
        createstartSector(),
        createSystem(System, Speicherplatz, datahandler, BlockSize)
    };
    Eintrag.isBootable = checkbootable(Eintrag);
    for (size_t i = 0; i < MAX_PARTITION_COUNT; i++) {
      if (partitions[i].firstSektor == NULL &&
          partitions[i].lastSektor == NULL) {
        partitions[i] = Eintrag;
        return partitions + i;
      }
    }
  std::cerr << "Die Partition konnte nicht erstellt werden, da die maximal Anzahl an Partitionen erreicht wurde" << std::endl;
    return nullptr;
  }

  unsigned char checkbootable(Partition E) { // überprüft ob eine Partition bootbar ist. Wird gesetzt wenn System gesetzt ist und First und Lastsektor gesetzt sind

    if (E.firstSektor != NULL && E.lastSektor != NULL && E.system != NULL) {
      return 0x80;
    } else {
      return 0;
    }
  }

  unsigned long long getStart() { // setzt den StartPtr
    if (partitions[0].startPtr == NULL) {
      return 0;
    } else {
      return checkLBA();
    }
  }
  unsigned long long checkLBA() { // berechnet wie viel Byte bisher belegt sind. Die Berechnung ist inspiriert durch die LBA Methode
    if (getPartitionCount() == 0) return 0;
    for (int i = 0; i < 5 + 1; i++) {
      if (i == MAX_PARTITION_COUNT || partitions[i].lastSektor == NULL) { // Loope bis kein Eintrag mehr gefunden wird um den letzten Eintrag zu ermitteln
        long c = partitions[i - 1].lastSektor->c;
        long h = partitions[i - 1].lastSektor->h;
        long s = partitions[i - 1].lastSektor->s;
        unsigned long long Ergebnis = 512 * ((c * maxHeads + h) * maxSectors + (s - 1));
        return Ergebnis;
      }
    }
    return 0;
  }

  CHS *createSector(unsigned long long speicherplatzInBytes, unsigned int BlockSize = 512) {
    // Erstelle eineN CHS Sektor
    CHS *sector = new CHS;
    unsigned int Speicherplatz = speicherplatzInBytes / BlockSize;
    unsigned long maxSpeicher =
        maxCylinders * maxHeads * maxSectors; // Todo Restspeicher abziehen
    if (Speicherplatz >= maxSpeicher) {
      std::cerr <<("Der gewünschte Speicherplatz liegt über dem Maximalwert der MBR von 8455716863 Bytes") << std::endl;
      return nullptr;
    }


      // Direkte Berechnung der CHS-Werte. Modulo Berechnung um die Reste zuweisen zu können
      sector->s = (Speicherplatz % maxSectors) + 1; // Sektoren starten bei 1,
      Speicherplatz /= maxSectors;
      sector->h = Speicherplatz % maxHeads;
      Speicherplatz /= maxHeads;
      sector->c = Speicherplatz;

      return sector;
    }

  CHS * createstartSector() { // Hilfsmethode um den Start Sektor zu ermitteln. Erstellt einen Sektor bei 0 0 1 wenn keine Einträge in partitions. Anosnsten wird ein Sektor direkt nach dem letzten LastSektor erstellt
    if (partitions[0].lastSektor == NULL && partitions[0].firstSektor == NULL) {
      return createSector(0);
    } else {
      return createSector(getStart());
    }
  }

  bool boot(int bIndex = 0) {
    if (partitions[bIndex].isBootable != 0) {
      return partitions[bIndex].isBootable != 1 &&
             partitions[bIndex].system->boot();
    }
    return 1;
  }
  unsigned long long checkSizeReserviert() { // Hilfsmethode um komplett genutzen Speicher zu nutzen
    unsigned long long MemoryAllocated = checkLBA();
    std::cout << "Es  ist bereits " << MemoryAllocated << " Byte reserviert" << std::endl;
    return MemoryAllocated;
    ;
  }
  unsigned long long checkPartitionsize(Partition* p) { // Berechnet die Size einer ganzer Partition indem first vom last Setkor abgezogen wird
    if (p == nullptr || p->firstSektor == nullptr|| p->lastSektor == nullptr) {
        return 0;
    }
    unsigned long long c1 = p->firstSektor->c;
    unsigned long long h1 = p->firstSektor->h;
    unsigned long long s1 = p->firstSektor->s;
    unsigned long long c2 = p->lastSektor->c;
    unsigned long long h2 = p->lastSektor->h;
    unsigned long long s2 = p->lastSektor->s;
    unsigned long long c = c2 - c1;
    unsigned long long h = h2 - h1;
    unsigned long long s = s2 - s1;
    unsigned long long Ergebnis = 512 * ((c * maxHeads + h) * maxSectors + (s - 1));
    std::cout << "Die Partition ist " << Ergebnis << " Byte groß" << std::endl;
    return Ergebnis;
  }
void deletePartition(unsigned int i) {
    if(i >= MAX_PARTITION_COUNT) {
        std::cerr << "Partition index übersteigt den Maximalenwert von " << static_cast<size_t>(MAX_PARTITION_COUNT) << std::endl;
    } else {
      partitions[i] = {0, SpeicherSystem::INODE_SYSTEM, 0, nullptr, nullptr, nullptr, nullptr};
      std::cout << "Die Partition an der Stelle " << i << " wurde gelöscht" << std::endl;
    }
  }
 /* void deletePartition(int i) { // Hilfsmethode um Partition zu löschen
    unsigned long long reserved[4] = {0};
    if (partitions[i].firstSektor == nullptr || partitions[i].lastSektor == nullptr || (i > 3 || i < 0)) {
      throw std::out_of_range("Es gibt diesen Eintrag nicht");
    } else {
      Partition p[4] = {0};
      for (int j = 0; j < 4 - 1; j++) {
        if (i != j && getSingularPartition(j) != nullptr) { // wenn j != i ist das ein Eintrag der nicht gelöscht oder verschoben werden muss
          reserved[j] = checkPartitionsize(*getSingularPartition(j));
          p->type = partitions[j].type;
        } else if (i == j && i < 5) { // überspringt den Eintrag der gelöscht werden soll und speichert die Größe der Einträge die nicht gelöscht werden müssen
          if (partitions[j + 1].firstSektor != NULL && getSingularPartition(j+1) != nullptr) {
            reserved[j] = checkPartitionsize(*getSingularPartition(j + 1));
            p[j].type = partitions[j + 1].type;
            i++;
          }
        }
      }
      for(int index = 0; index < MAX_PARTITION_COUNT; index++) {
        partitions[index] = {0};
      }



      for (int k = 0; k < 4; k++) {
        if (reserved[k] != 0) {
            if (p[k].system != nullptr)
                partitions[k] = createPartition(reserved[k], p[k].type, p[k].system->BLOCK_SIZE);
            else
                partitions[k] = createPartition(reserved[k], p[k].type);
        } else if (reserved[k] == 0) {
          partitions[k] = {0};
        }
      }
    }
  }  */

  unsigned long long getMaxSpeicherplatz() {
    return this->MaxSpeicherplatz;
  }
  Partition *getPartitions() { return partitions; }
  Partition* getSingularPartition(unsigned int i = 0) {
    if (i >= MAX_PARTITION_COUNT) {
      std::cerr << "Partition index übersteigt den Maximalenwert von 4" << std::endl;
      return nullptr;
    }
    return partitions + i;
  }
  // Erstellt ein System und gibt das zurück
  System *createSystem(SpeicherSystem System, unsigned int Speicher, Data *datahandler, BlockSizes BlockSize = BlockSizes::B_512) {
    void *memory = ::operator new(Speicher);
    if (System == BS_FAT) {
      return bootBSFat(memory, BlockSize, Speicher, datahandler);
    } else if (System == INODE_SYSTEM) {
      return bootINode(memory, BlockSize, Speicher, datahandler);
    } else {
        return nullptr;
    }
  }

  BsFat *bootBSFat(void *memory, BlockSizes blockSize, unsigned int memorySize, Data *dataHandler) {
    BsFat *B = BsFat::create(memory, memorySize, blockSize, dataHandler);
    return B;
  }
  INodeSystem *bootINode(void *memory, BlockSizes blockSize, unsigned int memorySize, Data *dataHandler) {
    INodeSystem *I =
        INodeSystem::create(memory, memorySize, blockSize, dataHandler);
    return I;
  }
  void setSystem(int index,System* s) {
    partitions[index].system = s;
  }

private:
  const int identificationCode = 0xAA55;
  unsigned long long MaxSpeicherplatz = 0;
  unsigned int sectorsCount = 0;
  unsigned int diskSignature = 0; // normalerweise nur in Windows
  Partition partitions[MAX_PARTITION_COUNT];
};

#endif // MBR_H
