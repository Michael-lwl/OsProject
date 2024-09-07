//
// Created by mehdi on 06.09.2024.
//

#ifndef ROM_H
#define ROM_H
#include "../../core/mbr.h"

class ROM{
  public:
   ROM(int Speicherplatz = 1000000000){
     this->Speicherplatz = Speicherplatz;
     MBR ROM(this->Speicherplatz);
     p = ROM.createPartition(Speicherplatz);

   }

   void ReadRom(std::string* filepath){
     p.system->getFile(filepath);
    }
   void WriteRom(std::string* filepath, unsigned long filesize,unsigned char flags){
     if(isBurned != 0){throw("Rom ist bereits gebrannt und kann nicht mehr beschrieben werden")}
     p.system->createFile(filepath,filesize,flags);
   }
   void burnRom(){
     if(isBurned != 0){
       throw("Rom ist bereits gebrannt");
     }
     else{isBurned = 1;}
   }
   void deleteFromRomBeforeBurn(std::string* filepath){
     if(isBurned != 0){
       throw("Rom ist bereits gebrannt und kann nicht bearbeitet werden");
     }
     else{
       p.system->deleteFile(filepath);
     }

   }

   private:
     bool isBurned = 0;
     int Speicherplatz;
     int MaxSpeicherplatz = 1000000000;
     Partition p;

};
#endif //ROM_H
