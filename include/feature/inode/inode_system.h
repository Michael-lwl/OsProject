#ifndef INODE_SYSTEM_H
#define INODE_SYSTEM_H

#include "./../../core/data.h"
#include "./../../core/data_sizes.h"
#include "./../../core/system.h"
#include "./../../core/directory.h"
#include "./inode.h"
#include <memory>


inline size_t mapToInodeSize(size_t driveSizeInByte) {
  if (driveSizeInByte <= ByteSizes::GB) {
    return INodeSizes::LE_1GB;
  }
  if (driveSizeInByte < 2 * ByteSizes::GB) {
    return INodeSizes::LT_2GB;
  }
  if (driveSizeInByte < 3 * ByteSizes::GB) {
    return INodeSizes::LT_3GB;
  }
  if (driveSizeInByte < ByteSizes::TB) {
    return INodeSizes::LE_1TB;
  }
  return INodeSizes::GT_1TB;
}

class INodeSystem : public System {
public:
  INodeSystem(unsigned char* startPtr, Data *dataHandler, size_t driveSizeInBytes, size_t blockSize) : System(dataHandler, driveSizeInBytes) {
    this->iNodeSize = mapToInodeSize(driveSizeInBytes);
    this->iNodeCount = driveSizeInBytes/iNodeSize;
    this->blockSize = blockSize;
    for (size_t i = 0; i < iNodeCount; i++) {
        //TODO: Verify that this works
        iNodes[i] = new (iNodes + i) INode(nullptr, 0, 0, blockSize);
    }
    auto datablockStartPtr = startPtr + sizeof(INodeSystem) + (sizeof(INode) * iNodeCount);

  }

  /// Deletes the file with the specified path, and only the specified file
  bool deleteFile(std::string *filePath);
  /// Returns the accessible free space of this partition
  unsigned long getFreeSpace();
  /// Returns the amount of stored files
  unsigned long getFileCount();
  /// Returns the size of the specified file
  unsigned long getFileSize(std::string *filePath);
  /// Creates a file on the specified path. It sets the flags and reserves the
  /// specified space. (If you want to save the content, you would have to save
  ///it via saveInFile afterwards) Generally strifes to be done in O(n)
  std::shared_ptr<File> createFile(std::string *filePath,
                                   unsigned long fileSize, unsigned char flags);
  /// Saves the content in the specified file, might resize it, if the reserved
  /// space is too small.
  bool saveInFile(std::string *filePath, std::shared_ptr<Array> data);
  /// Returns the file associated with the specified path.
  /// Might return null!
  std::shared_ptr<File> getFile(std::string *filePath);
  /// Returns the file with the INodes' id.
  /// Might return null!
  std::shared_ptr<File> getFile(unsigned long iNodeId);

  // Getter and setter
  /// Returns this Systems NULL_PTR
  u_int64_t *NULL_PTR();

private:
  /// Returns this Systems rootDirectory
  std::shared_ptr<File> getRoot();

  std::shared_ptr<File> getChild(std::shared_ptr<Directory> directory,
                                 const std::string &fileName);

  size_t iNodeSize;
  size_t iNodeCount;
  size_t blockSize;
  size_t driveSizeInBytes;

  INode* iNodes;
  DataBlock* dataBlocks;
};

#endif
