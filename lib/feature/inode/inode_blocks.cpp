#include "./../../../include/feature/inode/data_block.h"
#include "./../../../include/feature/inode/indirect_blocks.h"
#include <cstring>
#include <iostream>

/*
This Implements the setData(Array* data) methods for all (non-Abstract) Blocks
*/

bool DataBlock::setData(Array *data) {
  const unsigned int dataLen = data->getLength();
  if (dataLen > getBlockSize()) {
    std::cerr << "Cannot save data in this block: Capacity is too small!"
              << std::endl;
    return false;
  }
  std::strncpy((char *)this->data, (const char *)data, dataLen);
  for (unsigned int i = dataLen + 1; i < getBlockSize(); i++) {
    this->data[i] = 0;
  }
  return true;
}

Array DataBlock::getData() {
    
}

bool FirstIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getMaximumCapacity()) {
    std::cerr
        << "Cannot save data in this FirstIndirectBlock: Capacity is too small!"
        << std::endl;
    return false;
  }
  const unsigned int blockLen = this->getBlockSize();
  Array dataSlice = nullptr;
  unsigned int offset = 0;
  unsigned int curSize = maxSize;
  unsigned int i = 0;
  while (curSize > 0 && offset < maxSize && i < blockLen) {
    if (curSize >= blockLen) {
      dataSlice = new Array(blockLen, data->getArray() + offset,
                            MemAllocation::DONT_DELETE);
    } else {
      dataSlice = new Array(curSize, data->getArray() + offset,
                            MemAllocation::DONT_DELETE);
    }
    if (this->blocks[i] == nullptr) {
        this->blocks[i] = new DataBlock(getBlockSize());
    }
    if (!this->blocks[i]->setData(&dataSlice)) {
      std::cerr << "Cannot save data in DataBlock " << i << std::endl;
      return false;
    }
    i++;
    curSize -= blockLen;
    offset += blockLen;
  }
  this->setCurrentCapacity(data->getLength());
  return true;
}

Array FirstIndirectBlock::getData() {
    
}

bool SecondIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getMaximumCapacity()) {
    std::cerr << "Cannot save data in this SecondIndirectBlock: Capacity is "
                 "too small!"
              << std::endl;
    return false;
  }
  const unsigned int blockLen = this->getBlockSize();
  Array dataSlice = nullptr;
  unsigned int offset = 0;
  unsigned int curSize = maxSize;
  unsigned int i = 0;
  while (curSize > 0 && offset < maxSize && i < blockLen) {
    if (curSize >= blockLen) {
      dataSlice = new Array(blockLen, data->getArray() + offset, MemAllocation::DONT_DELETE);
    } else {
      dataSlice = new Array(curSize, data->getArray() + offset, MemAllocation::DONT_DELETE);
    }
    if (!(this->blocks[i].setData(&dataSlice))) {
      std::cerr << "Cannot save data in FirstIndirectBlock " << i << std::endl;
      return false;
    }
    i++;
    curSize -= blockLen;
    offset += blockLen;
  }
  this->setCurrentCapacity(data->getLength());
  return true;
}

Array SecondIndirectBlock::getData() {
    
}

bool ThirdIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getMaximumCapacity()) {
    std::cerr
        << "Cannot save data in this ThirdIndirectBlock: Capacity is too small!"
        << std::endl;
    return false;
  }
  const unsigned int blockLen = this->getBlockSize();
  Array dataSlice = nullptr;
  unsigned int offset = 0;
  unsigned int curSize = maxSize;
  unsigned int i = 0;
  while (curSize > 0 && offset < maxSize && i < blockLen) {
    if (curSize >= blockLen) {
      dataSlice = new Array(blockLen, data->getArray() + offset, MemAllocation::DONT_DELETE);
    } else {
      dataSlice = new Array(curSize, data->getArray() + offset, MemAllocation::DONT_DELETE);
    }
    if (!(this->blocks[i].setData(&dataSlice))) {
      std::cerr << "Cannot save data in SecondIndirectBlock " << i << std::endl;
      return false;
    }
    i++;
    curSize -= blockLen;
    offset += blockLen;
  }
  this->setCurrentCapacity(data->getLength());
  return true;
}

Array ThirdIndirectBlock::getData() {
    
}
