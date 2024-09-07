#include "./../include/utils.h"
#include "./../include/core/data_impl.h"
#include "./../include/core/data_sizes.h"
#include "./../include/core/mbr.h"
#include "./../include/feature/fat/bs_system.h"
#include "./../include/feature/inode/inode_system.h"
#include "./../include/feature/ui/mainwindow.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <string.h>
#include <string>

#define TODO *SysOut() << "TODO: implement the rest" << std::endl;
void simpleAndClean(void *ptr) {
  if (ptr != nullptr)
    free(ptr);
}

int colouredOutputTest() {
  *SysOut() << colorize("T", Color::BLUE) << "es" << colorize("t", Color::RED)
            << std::endl;
  return 0;
}

int test_BsFat(unsigned long long memorySize, BlockSizes blockSize = BlockSizes::B_512) {
  using namespace std;
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

  *SysOut() << "Filesize1: " << file1Size << "\nFilesize2: " << file2Size
       << "\nBlockSize: " << blockSize << "\nBlockCount: " << pFat->getBlockCount() <<endl;

  *SysOut() << "Creating File 1" << endl;
  shared_ptr<File> file1 =
      pFat->createFile(&filename1, file1Size, Flags::SYSTEM | Flags::IS_TEMP);
  if (file1 == nullptr) {
    cerr << "Failed to create file1.tmp." << endl;
  }
  *SysOut() << "Created File 1" << endl;
  pFat->show();

  shared_ptr<File> file2 =
      pFat->createFile(&filename2, file2Size, Flags::ASCII);
  if (file2 == nullptr) {
    cerr << "Failed to create file2.txt." << endl;
  }
  pFat->show();
  *SysOut() << "--------------------------------------------------------------------"
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
    *SysOut() << "Deleting file1" << endl;
    file1.reset();
    pFat->deleteFile(&filename1);
    pFat->show();
  }

  // Check fragmentation before defragmentation
  float fragmentationBefore = pFat->getFragmentation();
  *SysOut() << "Fragmentation before defrag BsFat: " << fragmentationBefore << endl;

  // Defragment the disk
  *SysOut() << "Defragmenting disk" << endl;
  pFat->defragDisk();
  pFat->show();

  // Check fragmentation after defragmentation
  float fragmentationAfter = pFat->getFragmentation();
  *SysOut() << "Fragmentation after defrag BsFat: " << fragmentationAfter << endl;

  file2.reset();
  delete pFat;

  return 0;
}

int test_INodes(unsigned long long memorySize, BlockSizes blockSize = BlockSizes::B_512) {
  using namespace std;
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

  *SysOut() << "Filesize1: " << file1Size << "\nFilesize2: " << file2Size
       << "\nBlockSize: " << blockSize << "\nBlockCount: " << iNodeSystem->getBlockCount() <<endl;

  *SysOut() << "Creating File 1" << endl;
  shared_ptr<File> file1 =
      iNodeSystem->createFile(&filename1, file1Size, Flags::SYSTEM | Flags::IS_TEMP);
  if (file1 == nullptr) {
    cerr << "Failed to create file1.tmp." << endl;
  }
  *SysOut() << "Created File 1" << endl;
  iNodeSystem->show();

  *SysOut() << "Creating File 2" << endl;
  shared_ptr<File> file2 =
      iNodeSystem->createFile(&filename2, file2Size, Flags::ASCII);
  if (file2 == nullptr) {
    cerr << "Failed to create file2.txt." << endl;
  }
  *SysOut() << "Created File 2" << endl;
  iNodeSystem->show();
  *SysOut() << "--------------------------------------------------------------------"
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
    *SysOut() << "Deleting file1" << endl;
    file1.reset();
    iNodeSystem->deleteFile(&filename1);
    iNodeSystem->show();
  }

  // Check fragmentation before defragmentation
  float fragmentationBefore = iNodeSystem->getFragmentation();
  *SysOut() << "Fragmentation before defrag INodeSystem: " << fragmentationBefore << endl;

  // Defragment the disk
  *SysOut() << "Defragmenting disk" << endl;
  iNodeSystem->defragDisk();
  iNodeSystem->show();

  // Check fragmentation after defragmentation
  float fragmentationAfter = iNodeSystem->getFragmentation();
  *SysOut() << "Fragmentation after defrag INodeSystem: " << fragmentationAfter << endl;

  file2.reset();
  delete iNodeSystem;

  return 0;
}

int test_mbr() {
  MBR mbr(8455716863);
  std::cout << "Erstellung mBR erfolgreich" << std::endl;
  mbr.createPartition(2000000);
  mbr.createPartition(1000000);
  mbr.createPartition(6000000,INODE);
  std::cout << "Erstellung P1 erfolgreich" << std::endl;
  Partition p = mbr.getSingularPartition(0);
  mbr.checkPartitionsize(p);
  mbr.checkSizeReserviert();
  mbr.getPartition();
  Partition p2 = mbr.getSingularPartition(1);
  std::cout << "Partition selektiert" << std::endl;
  mbr.deletePartition(0);
  Partition p3 = mbr.getSingularPartition(1);
  std::cout << "Die Partition hat" << mbr.checkPartitionsize(p2) << "Bytes" << std::endl;;
  std::cout << "Die Partition hat" << mbr.checkPartitionsize(p) << "Bytes" << std::endl;

  return 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
  // int output = 0;
  // BlockSizes blockSize = BlockSizes::B_512;
  // unsigned long long memorySize = 8 * getSizeInByte(ByteSizes::MiB); // 8 MebiByte
  // output |= test_BsFat(memorySize, blockSize);
  // output |= test_INodes(memorySize, blockSize);
  // return output;
  QApplication app(argc, argv);

  MainWindow mainWindow;
  mainWindow.setWindowTitle("OsProject");
  mainWindow.showMaximized();

  return app.exec();
}
