#include "./../../../include/feature/inode/inode_system.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

bool INode::setData(Array *data) {
  using namespace std;
  if (data == nullptr || data->getLength() == 0) {
    this->resizeFile(0);
    return true;
  }
  unsigned int dataLen = data->getLength();
  if (data->getLength() != this->getFileSizeInBytes()) {
    if (!resizeFile(data->getLength())) {
      cerr << "Cannot resize File \"" << this->getFilePath()
           << "\" to size:" << data->getLength() << endl;
    }
  }
  size_t interval = datablocks[0]->BLOCK_SIZE;
  size_t counter = 0;
  for (size_t dataBlockIndex = 0;
       counter < dataLen && dataBlockIndex < DIRECT_DATA_BLOCK_COUNT;
       counter += interval) {
    Array curData = new Array(interval, data->getArray() + counter,
                              MemAllocation::DONT_DELETE);
    this->datablocks[dataBlockIndex++]->setData(&curData);
  }

  size_t remainingDataLen = dataLen - counter;
  if (remainingDataLen == 0) { // if the remainder is 0, we are finished!
    return true;
  }

  /// This will be used for loop usage of getData
  IndirectBlock *indirBlocks[INDIRECTION_BLOCK_DEPTH] = {
      this->firstIndirectionBlock, this->secondIndirectionBlock,
      this->thirdIndirectionBlock};

  for (size_t i = 0; i < INDIRECTION_BLOCK_DEPTH; i++) {
    remainingDataLen = dataLen - counter;
    Array curData = new Array(interval, data->getArray() + counter,
                              MemAllocation::DONT_DELETE);
    if (!indirBlocks[i]->setData(&curData)) {
      cerr << "Could not save data in File!" << endl;
      return false;
    }
    counter += interval;
  }

  return true;
}

std::unique_ptr<Array> INode::getData() {
  using namespace std;
  unique_ptr<Array> data = make_unique<Array>(this->getFileSizeInBytes());
  size_t dataLen = data->getLength();
  if (dataLen == 0) {
    return data;
  }
  size_t interval = this->datablocks[0]->BLOCK_SIZE;
  size_t counter = 0;
  size_t dataBlockIndex = 0;
  while (counter < dataLen - interval &&
         dataBlockIndex < DIRECT_DATA_BLOCK_COUNT) {
    strncpy(reinterpret_cast<char *>(data->getArray() + counter),
            reinterpret_cast<char *>(
                this->datablocks[dataBlockIndex]->getData().getArray()),
            interval);
    counter += interval;
    dataBlockIndex++;
  }
  // If the first datablocks were not fully copied
  // because the dataLen is not a multiple of the blocksize)
  if (counter < dataLen && dataBlockIndex < DIRECT_DATA_BLOCK_COUNT) {
    size_t remainder = dataLen % counter;
    strncpy(reinterpret_cast<char *>(data->getArray() + counter),
            reinterpret_cast<char *>(
                this->datablocks[dataBlockIndex]->getData().getArray()),
            remainder);
    counter += remainder;
  }
  // If the file was completely read, return early.
  if (dataLen <= counter) {
    return data;
  }

  /// This will be used for loop usage of getData
  IndirectBlock *indirBlocks[INDIRECTION_BLOCK_DEPTH] = {
      this->firstIndirectionBlock, this->secondIndirectionBlock,
      this->thirdIndirectionBlock};

  for (size_t i = 0; i < INDIRECTION_BLOCK_DEPTH; i++) {
    Array curData = indirBlocks[i]->getData();
    size_t remainingSpace = dataLen - counter;
    if (curData.getLength() <= remainingSpace) {
      strncpy(reinterpret_cast<char *>(data->getArray() + counter),
              reinterpret_cast<char *>(curData.getArray()),
              curData.getLength());
      counter += curData.getLength();
    } else {
      strncpy(reinterpret_cast<char *>(data->getArray() + counter),
              reinterpret_cast<char *>(curData.getArray()), remainingSpace);
      cerr << "WARNING: Actual file size is bigger than its metadata suggests. "
              "File might be corrupt!"
           << endl;

      // TODO: mark this file corrupt?
      counter += remainingSpace;
      return data;
    }
  }
  // TODO: Testing!

  return data;
}

bool INode::trimToSize(unsigned long newFileSize) {
  if (getFileSizeInBytes() < newFileSize)
    return false;
  if (newFileSize == getFileSizeInBytes())
    return true;

  unsigned long plusOne = (newFileSize % system->BLOCK_SIZE != 0 ? 1 : 0);
  unsigned long newBlockCount =
      (newFileSize / (system->BLOCK_SIZE)) + (plusOne);

  IndirectBlock *indirBlocks[3] =
  { firstIndirectionBlock,
    secondIndirectionBlock,
    thirdIndirectionBlock };
  size_t indirCap[3] =
  { system->BLOCK_SIZE, system->BLOCK_SIZE * system->BLOCK_SIZE,
      system->BLOCK_SIZE * system->BLOCK_SIZE * system->BLOCK_SIZE};

  if (newBlockCount < DIRECT_DATA_BLOCK_COUNT) {
    for (size_t i = newBlockCount; i < DIRECT_DATA_BLOCK_COUNT; i++) {
      if (datablocks[i] != nullptr) {
        datablocks[i]->setData(&Array::EMPTY_ARRAY);
        datablocks[i] = nullptr;
      }
    }
    for (IndirectBlock* indirBlock : indirBlocks){
        indirBlock->setData(&Array::EMPTY_ARRAY);
    }
  }
  ///TODO rework!

  return true;
}

bool INode::expandToSize(unsigned long newFileSize) {
  if (newFileSize < getFileSizeInBytes()) {
    return false;
  }
  if (this->system->getFreeSpace() < newFileSize) {
    std::cerr << "Error: cannot increase file size: System-space is too full"
              << std::endl;
    return false;
  }
  if (this->getFileSizeInBytes() == newFileSize) {
    return true;
  }

  long remainingSize = newFileSize - getFileSizeInBytes();
  size_t interval = system->BLOCK_SIZE;
  DataBlock* newDataBlock;
  while (0 < remainingSize) {
    newDataBlock = system->getNewDataBlock();
    if (newDataBlock == nullptr) {
      std::cerr << "Error: cannot increase file size: could not get enough "
                   "free blocks"
                << std::endl;
      return false;
    }
    if (!appendDataBlock(newDataBlock)) {
      return false;
    }
    remainingSize -= interval;
  }
  return true;
}

bool INode::appendDataBlock(DataBlock* db) {
  if (db == nullptr) {
    return false;
  }
  for (unsigned int i = 0; i < DIRECT_DATA_BLOCK_COUNT; i++) {
    if (datablocks[i]->status == Status::FREE) {
      datablocks[i] = db;
      datablocks[i]->status &= ~Status::FREE;
      return true;
    }
  }
  if (firstIndirectionBlock->appendDataBlock(db, this->system)) {
    return true;
  }
  if (secondIndirectionBlock->appendDataBlock(db, this->system)) {
    return true;
  }
  if (thirdIndirectionBlock->appendDataBlock(db, this->system)) {
    return true;
  }
  return false;
}

std::unique_ptr<Array> INodeDirectory::getData() {
  std::vector<INode *> files = getChildren();
  size_t deSize = sizeof(DirectoryEntry);
  size_t len = deSize * files.size();
  std::unique_ptr<Array> dirContent = std::make_unique<Array>(len);

  unsigned char *curPtr = dirContent->getArray();

  for (auto f : files) {
    std::strncpy(reinterpret_cast<char *>(curPtr), reinterpret_cast<char *>(f),
                 deSize);
    curPtr += deSize;
  }

  return dirContent;
}

std::vector<INode *> INodeDirectory::getChildren() {
  std::vector<INode *> output;
  for (DirectoryEntry de : files) {
    output.push_back((INode *)de.inode_id);
  }
  return output;
}

bool INodeDirectory::addChild(std::shared_ptr<File> file) {
  DirectoryEntry de = {(size_t)file.get(), *file->getFilePath()};
  size_t newSize = INode::getFileSizeInBytes() + sizeof(de);
  this->INode::setFileSizeInBytes(newSize);
  this->files.push_back(de);
  return true;
}

bool INodeDirectory::removeChild(std::string filename) {
  for (size_t i = 0; i < files.size(); i++) {
    if (files[i].name == filename) {
      files.erase(files.begin() + i);
      return true;
    }
  }
  std::cerr << "No File in Directory \""
            << static_cast<INode>(this).getFilePath() << "\"with name: \""
            << filename << "\"!" << std::endl;
  return false;
}

bool INodeDirectory::removeChild(size_t childId) {
  for (size_t i = 0; i < files.size(); i++) {
    if (files[i].inode_id == childId) {
      files.erase(files.begin() + i);
      return true;
    }
  }
  std::cerr << "No File in Directory \""
            << static_cast<INode>(this).getFilePath()
            << "\"with id: " << childId << "!" << std::endl;
  return false;
}
