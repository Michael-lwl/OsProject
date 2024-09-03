#include "../include/utils.h"
#include "./../include/core/data_impl.h"
#include "./../include/core/data_sizes.h"
#include "./../include/feature/fat/bs_system.h"
#include "./../include/core/mbr.h"
#include <cmath>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

#define TODO std::cout << "TODO: implement the rest" << std::endl;
void simpleAndClean(void *ptr) {
  if (ptr != nullptr)
    free(ptr);
}
/*
int colouredOutputTest() {
  std::cout << colorize("T", Color::BLUE) << "es" << colorize("t", Color::RED)
            << std::endl;
  return 0;
}

int test_BsFat() {
  using namespace std;
  int blockSize = 512;
  int memorySize = 1048576; // 8 MebiByte
  Data *dataHandler = new Data_Impl(blockSize);
  BsFat *pFat = BsFat::create(memorySize, blockSize, dataHandler);
  // createBsFat(memorySize, blockSize);
  if (!pFat) {
    cerr << "Failed to create BsFat." << endl;
    return -1;
  }

  // Show initial FAT state
  pFat->showFat();

  std::string filename1 = "file1.txt";
  unsigned long long file1Size = 200 * blockSize;
  std::string filename2 = "file2.txt";
  unsigned long long file2Size = 50 * blockSize;

  cout << "Filesize1: " << file1Size << "\nFilesize2: " << file2Size
       << "\nBlockSize: " << blockSize << "\nBlockCount: " << pFat->getBlockCount() <<endl;

  cout << "Creating File 1" << endl;
  shared_ptr<File> file1 =
      pFat->createFile(&filename1, file1Size, Flags::ASCII);
  if (file1 == nullptr) {
    cerr << "Failed to create file1.txt." << endl;
  }
  cout << "Created File 1" << endl;
  pFat->showFat();

  shared_ptr<File> file2 =
      pFat->createFile(&filename2, file2Size, Flags::SYSTEM);
  if (file2 == nullptr) {
    cerr << "Failed to create file2.txt." << endl;
  }
  pFat->showFat();
  cout << "--------------------------------------------------------------------"
          "--------------------------------------------------------------------"
       << endl;

  // Corrupt a random block
  auto corruptedCluster = pFat->getNewCluster();
  if (corruptedCluster != nullptr)
    corruptedCluster->status = Status::CORRUPTED;

  // Reserve a random block
  auto reservedCluster = pFat->getNewCluster();
  if (reservedCluster != nullptr)
    reservedCluster->status = Status::RESERVED;

  pFat->showFat();
  if (file1 != nullptr) {
    // Delete the first file
    cout << "Deleting file1" << endl;
    pFat->deleteFile(&filename1);
    pFat->showFat();
  }

  // Check fragmentation before defragmentation
  float fragmentationBefore = pFat->getFragmentation();
  cout << "Fragmentation before defrag: " << fragmentationBefore << endl;

  // Defragment the disk
  cout << "Defragmenting disk" << endl;
  pFat->defragDisk();
  pFat->showFat();

  // Check fragmentation after defragmentation
  float fragmentationAfter = pFat->getFragmentation();
  cout << "Fragmentation after defrag: " << fragmentationAfter << endl;

  delete pFat;

  return 0;
} */
int test_mbr() {
  Master_Boot_Record m(2000,512);
  SpeicherSystem b = BS_FAT;
  m.createPartition(b);
  m.createPartition(b);
  return 1;

}

const size_t STD_BLOCK_SIZE = 4 * getSizeInByte(ByteSizes::KB);

int main() {
 /*   int argc, char **argv
  *   (void) argc;
    (void) argv;
  std::cerr << STD_BLOCK_SIZE << std::endl;
 unsigned char* FestePlatte;
  if (hasFile) {
      FestePlatte = readFile(filename);
  } else {
      FestePlatte = static_cast<unsigned char*>(malloc(sizeof(char) *
  ByteSizes::GB));
  }
  MBR* mbr = new (FestePlatte) MBR();
  Data* dh1 = new Data_Impl(120);
  Data* dh2 = new Data_Impl(120);
  const unsigned char* usableDrive = FestePlatte + sizeof(MBR) + 1;
  INodeSystem* inode1 = new (usableDrive) INodeSystem()
  mbr.addSystem(festePlatte[0]);*/
  //return test_BsFat();
  std::cout << "hi";
 // std::cout << "hi";
  return test_mbr();
}
