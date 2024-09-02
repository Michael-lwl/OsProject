#include "./../../../include/feature/inode/data_block.h"
#include "./../../../include/feature/inode/indirect_blocks.h"
#include <cstring>
#include <iostream>
#include <memory>

/*
This Implements the setData(Array* data) methods for all (non-Abstract) Blocks
*/

bool DataBlock::setData(Array *data) {
  const unsigned int dataLen = data->getLength();
  if (dataLen > BLOCK_SIZE) {
    std::cerr << "Cannot save data in this block: Capacity is too small!"
              << std::endl;
    return false;
  }
  std::strncpy((char *)this->data, (const char *)data, dataLen);
  for (unsigned int i = dataLen + 1; i < BLOCK_SIZE; i++) {
    this->data[i] = 0;
  }
  return true;
}

Array DataBlock::getData() {
  Array output = new Array(BLOCK_SIZE);
  std::strncpy((char *)output.getArray(), (const char *)this->data, BLOCK_SIZE);
  return output;
}

bool FirstIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getMaximumCapacity()) {
    std::cerr
        << "Cannot save data in this FirstIndirectBlock: Capacity is too small!"
        << std::endl;
    return false;
  }
  if (maxSize == 0) {//To clear the data, we take a array with size 0
      Array* emptyData = new Array(BLOCK_SIZE);
      for (size_t i = 0; i < getCurrentCapacity(); i++) {
          if (this->blocks[i] != nullptr) {
              this->blocks[i]->setData(emptyData);
              this->blocks[i] = nullptr;
          }
      }
      return true;
  }
  const unsigned int blockLen = this->BLOCK_SIZE;
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
      this->blocks[i] = new DataBlock(BLOCK_SIZE);
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
  size_t cap = getCurrentCapacity();
  size_t countBlocks = cap / BLOCK_SIZE;
  Array output = new Array(cap);
  char *curP = (char *)output.getArray();
  for (size_t i = 0; i < countBlocks; i++) {
    std::strncpy(curP, (const char *)this->blocks[i]->getData().getArray(),
                 BLOCK_SIZE);
    curP += BLOCK_SIZE;
  }

  return output;
}

bool FirstIndirectBlock::appendDataBlock(std::shared_ptr<DataBlock> block) {
  size_t curCap = this->getCurrentCapacity();
  if (curCap >= this->getMaximumCapacity()) {
    return false;
  }
  this->blocks[curCap] = block.get();
  this->setCurrentCapacity(curCap + 1);
  return true;
}

bool SecondIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getMaximumCapacity()) {
    std::cerr << "Cannot save data in this SecondIndirectBlock: Capacity is "
                 "too small!"
              << std::endl;
    return false;
  }
  if (maxSize == 0) {//To clear the data, we take a array with size 0
      for (size_t i = 0; i < getCurrentCapacity(); i++) {
          this->blocks[i].setData(&Array::EMPTY_ARRAY);
      }
      return true;
  }
  const unsigned int blockLen = this->BLOCK_SIZE;
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
  size_t cap = getCurrentCapacity();
  size_t countBlocks = cap / BLOCK_SIZE;
  Array output = new Array(cap);
  char *curP = (char *)output.getArray();
  for (size_t i = 0; i < countBlocks; i++) {
    std::strncpy(curP, (const char *)this->blocks[i].getData().getArray(),
                 BLOCK_SIZE);
    curP += BLOCK_SIZE;
  }

  return output;
}

bool SecondIndirectBlock::appendDataBlock(std::shared_ptr<DataBlock> block) {
  size_t curCap = this->getCurrentCapacity();
  if (curCap >= this->getMaximumCapacity()) {
    return false;
  }
  if (!this->blocks[curCap].appendDataBlock(block)) {
    this->blocks[curCap + 1].appendDataBlock(block);
  }
  this->setCurrentCapacity(curCap + 1);
  return true;
}

bool ThirdIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getMaximumCapacity()) {
    std::cerr
        << "Cannot save data in this ThirdIndirectBlock: Capacity is too small!"
        << std::endl;
    return false;
  }
  if (maxSize == 0) {//To clear the data, we take a array with size 0
      for (size_t i = 0; i < getCurrentCapacity(); i++) {
          this->blocks[i].setData(&Array::EMPTY_ARRAY);
      }
      return true;
  }
  const unsigned int blockLen = this->BLOCK_SIZE;
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
  size_t cap = getCurrentCapacity();
  size_t countBlocks = cap / BLOCK_SIZE;
  Array output = new Array(cap);
  char *curP = (char *)output.getArray();
  for (size_t i = 0; i < countBlocks; i++) {
    std::strncpy(curP, (const char *)this->blocks[i].getData().getArray(),
                 BLOCK_SIZE);
    curP += BLOCK_SIZE;
  }

  return output;
}

bool ThirdIndirectBlock::appendDataBlock(std::shared_ptr<DataBlock> block) {
  size_t curCap = this->getCurrentCapacity();
  if (curCap >= this->getMaximumCapacity()) {
    return false;
  }
  if (!this->blocks[curCap].appendDataBlock(block)) {
    this->blocks[curCap + 1].appendDataBlock(block);
  }
  this->setCurrentCapacity(curCap + 1);
  return true;
}
