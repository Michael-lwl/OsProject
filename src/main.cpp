#include "../include/utils.h"
#include "./../include/core/data_impl.h"
#include "./../include/core/data_sizes.h"
#include "./../include/feature/fat/bs_system.h"
#include "./../include/feature/inode/inode_system.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <string.h>
#include <string>

#define TODO std::cout << "TODO: implement the rest" << std::endl;
void simpleAndClean(void *ptr) {
  if (ptr != nullptr)
    free(ptr);
}

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
  // Allocate memory for the entire structure (BsFat + all clusters + data blocks)
  void* memory = ::operator new(memorySize);  // Raw allocation
  BsFat *pFat = BsFat::create(memory, memorySize, blockSize, dataHandler);

  // createBsFat(memorySize, blockSize);
  if (!pFat) {
    cerr << "Failed to create BsFat." << endl;
    return -1;
  }

  // Show initial FAT state
  pFat->show();

  std::string filename1 = "file1.tmp";
  unsigned long long file1Size = 200 * blockSize;
  std::string filename2 = "file2.txt";
  unsigned long long file2Size = 50 * blockSize;

  cout << "Filesize1: " << file1Size << "\nFilesize2: " << file2Size
       << "\nBlockSize: " << blockSize << "\nBlockCount: " << pFat->getBlockCount() <<endl;

  cout << "Creating File 1" << endl;
  shared_ptr<File> file1 =
      pFat->createFile(&filename1, file1Size, Flags::SYSTEM | Flags::IS_TEMP);
  if (file1 == nullptr) {
    cerr << "Failed to create file1.tmp." << endl;
  }
  cout << "Created File 1" << endl;
  pFat->show();

  shared_ptr<File> file2 =
      pFat->createFile(&filename2, file2Size, Flags::ASCII);
  if (file2 == nullptr) {
    cerr << "Failed to create file2.txt." << endl;
  }
  pFat->show();
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

  pFat->show();
  if (file1 != nullptr) {
    // Delete the first file
    cout << "Deleting file1" << endl;
    pFat->deleteFile(&filename1);
    pFat->show();
  }

  // Check fragmentation before defragmentation
  float fragmentationBefore = pFat->getFragmentation();
  cout << "Fragmentation before defrag: " << fragmentationBefore << endl;

  // Defragment the disk
  cout << "Defragmenting disk" << endl;
  pFat->defragDisk();
  pFat->show();

  // Check fragmentation after defragmentation
  float fragmentationAfter = pFat->getFragmentation();
  cout << "Fragmentation after defrag: " << fragmentationAfter << endl;

  delete pFat;

  return 0;
}

int test_INodes() {
  using namespace std;
  int blockSize = 512;
  int memorySize = 1048576; // 8 MebiByte
  Data *dataHandler = new Data_Impl(blockSize);
  void* memory = ::operator new(memorySize);  // Raw memory allocation
  INodeSystem *iNodeSystem = INodeSystem::create(memory, memorySize, blockSize, dataHandler);
  // createBsFat(memorySize, blockSize);
  if (!iNodeSystem) {
    cerr << "Failed to create INodeSystem." << endl;
    return -1;
  }

  // Show initial FAT state
  iNodeSystem->show();

  std::string filename1 = "file1.tmp";
  unsigned long long file1Size = 200 * blockSize;
  std::string filename2 = "file2.txt";
  unsigned long long file2Size = 50 * blockSize;

  cout << "Filesize1: " << file1Size << "\nFilesize2: " << file2Size
       << "\nBlockSize: " << blockSize << "\nBlockCount: " << iNodeSystem->getBlockCount() <<endl;

  cout << "Creating File 1" << endl;
  shared_ptr<File> file1 =
      iNodeSystem->createFile(&filename1, file1Size, Flags::SYSTEM | Flags::IS_TEMP);
  if (file1 == nullptr) {
    cerr << "Failed to create file1.tmp." << endl;
  }
  cout << "Created File 1" << endl;
  iNodeSystem->show();

  cout << "Creating File 2" << endl;
  shared_ptr<File> file2 =
      iNodeSystem->createFile(&filename2, file2Size, Flags::ASCII);
  if (file2 == nullptr) {
    cerr << "Failed to create file2.txt." << endl;
  }
  cout << "Created File 2" << endl;
  iNodeSystem->show();
  cout << "--------------------------------------------------------------------"
          "--------------------------------------------------------------------"
       << endl;

  // Corrupt a random block
  auto corruptedDataBlock = iNodeSystem->getNewDataBlock();
  if (corruptedDataBlock != nullptr)
    corruptedDataBlock->status = Status::CORRUPTED;

  // Reserve a random block
  auto reservedDataBlock = iNodeSystem->getNewDataBlock();
  if (reservedDataBlock != nullptr)
    reservedDataBlock->status = Status::RESERVED;

  iNodeSystem->show();
  if (file1 != nullptr) {
    // Delete the first file
    cout << "Deleting file1" << endl;
    iNodeSystem->deleteFile(&filename1);
    iNodeSystem->show();
  }

  // Check fragmentation before defragmentation
  float fragmentationBefore = iNodeSystem->getFragmentation();
  cout << "Fragmentation before defrag: " << fragmentationBefore << endl;

  // Defragment the disk
  cout << "Defragmenting disk" << endl;
  iNodeSystem->defragDisk();
  iNodeSystem->show();

  // Check fragmentation after defragmentation
  float fragmentationAfter = iNodeSystem->getFragmentation();
  cout << "Fragmentation after defrag: " << fragmentationAfter << endl;

  delete iNodeSystem;

  return 0;
}

const size_t STD_BLOCK_SIZE = 4 * getSizeInByte(ByteSizes::KB);

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
  std::cerr << STD_BLOCK_SIZE << std::endl;
  /*unsigned char* FestePlatte;
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
  int output = 0;
  // output |= test_BsFat();
  output |= test_INodes();
  return output;
}
