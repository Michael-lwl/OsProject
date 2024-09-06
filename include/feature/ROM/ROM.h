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

   void WriteRom(){

    }
   void ReadRom(){}
   void burnRom(){
     if(isBurned != 0){
       throw("Rom ist bereits gebrannt");
     }
     else{isBurned = 1;}
   }
   void deleteFromRomBeforeBurn(){}

   private:
     bool isBurned = 0;
     int Speicherplatz;
     int MaxSpeicherplatz = 1000000000;
     Partition p;

}
#endif //ROM_H
