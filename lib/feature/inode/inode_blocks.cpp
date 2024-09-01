#include "./../../../include/feature/inode/inode_blocks.h"
#include <cstring>
#include <iostream>
#include <ostream>

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
  if (maxSize > BLOCK_SIZE * BLOCK_SIZE) {
    std::cerr
        << "Cannot save data in this FirstIndirectBlock: Capacity is too small!"
        << std::endl;
    return false;
  }
  if (maxSize == 0) {//To clear the data, we take a array with size 0
      Array* emptyData = new Array(BLOCK_SIZE);
      for (unsigned long i = 0; i < getLength(); i++) {
          if (this->data[i] != nullptr) {
              this->data[i]->setData(emptyData);
              this->data[i] = nullptr;
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
    if (this->data[i] == nullptr) {
      this->data[i] = new DataBlock(BLOCK_SIZE);
    }
    if (!this->data[i]->setData(&dataSlice)) {
      std::cerr << "Cannot save data in DataBlock " << i << std::endl;
      return false;
    }
    i++;
    curSize -= blockLen;
    offset += blockLen;
  }
  return true;
}

Array FirstIndirectBlock::getData() {
  unsigned long cap = getLength();
  unsigned long countBlocks = cap / BLOCK_SIZE;
  Array output = new Array(cap);
  char *curP = (char *)output.getArray();
  for (unsigned long i = 0; i < countBlocks; i++) {
      if (this->data[i] != nullptr) {
          std::strncpy(curP, (const char *)this->data[i]->getData().getArray(),
                 BLOCK_SIZE);
        curP += BLOCK_SIZE;
      }
  }

  return output;
}

bool FirstIndirectBlock::appendDataBlock(DataBlock* block, INodeSystem* system) {
  unsigned long curCap = this->getLength();
  if (curCap >= this->getCapacity()) {
    return false;
  }
  this->data[curCap] = block;
  return true;
}

bool FirstIndirectBlock::trimToSize(size_t newSize) {
    if (newSize > this->getByteCapacity())
        return false;
    size_t curCap = this->getLength();
    if (curCap < newSize)
        return false;
    size_t newBlockCount = newSize / BLOCK_SIZE + (newSize % BLOCK_SIZE == 0 ? 0 : 1);//If newSize is a bytecount, we maybe need to append one more
    if (newBlockCount > this->getCapacity())
        return false;
    for (size_t curBlock = newBlockCount; curBlock < curCap; curBlock++) {
        if (data[curBlock] != nullptr) {
            data[curBlock]->status = Status::FREE;
            data[curBlock] = nullptr;
        }
    }
    return true;
}

unsigned long long FirstIndirectBlock::getLength() {
    unsigned long long output = 0;
    for (size_t i = 0; i < this->BLOCK_SIZE; i++) {
        if (this->data[i] == nullptr) {
            return ++i;
        }
    }
    return this->BLOCK_SIZE;
}

bool SecondIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE) {
    std::cerr << "Cannot save data in this SecondIndirectBlock: Capacity is "
                 "too small!"
              << std::endl;
    return false;
  }
  if (maxSize == 0) {//To clear the data, we take a array with size 0
      for (unsigned long i = 0; i < this->getLength(); i++) {
          this->data[i]->setData(&Array::EMPTY_ARRAY);
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
    if (!(this->data[i]->setData(&dataSlice))) {
      std::cerr << "Cannot save data in FirstIndirectBlock " << i << std::endl;
      return false;
    }
    i++;
    curSize -= blockLen;
    offset += blockLen;
  }
  return true;
}

Array SecondIndirectBlock::getData() {
  unsigned long cap = this->getLength();
  unsigned long countBlocks = cap;
  Array output = new Array(cap * BLOCK_SIZE);
  char *curP = (char *)output.getArray();
  for (unsigned long i = 0; i < countBlocks; i++) {
    std::strncpy(curP, reinterpret_cast<const char *>(this->data[i]->getData().getArray()),
                 BLOCK_SIZE);
    curP += BLOCK_SIZE;
  }

  return output;
}

bool SecondIndirectBlock::appendDataBlock(DataBlock* block, INodeSystem* system) {
  unsigned long curCap = this->getLength();
  if (curCap >= this->getCapacity()) {
    return false;
  }
  if (this->data[curCap] == nullptr) {
      this->data[curCap] = system->getNewFirstIndirectBlock();
      if (this->data[curCap] == nullptr) {
          std::cerr << "Cannot allocate new FirstIndirectionBlock: Drive might be full!" << std::endl;
          return false;
      }
  }
  if (!this->data[curCap++]->appendDataBlock(block, system)) {// WARNING: Increases curCap after this call!
      if (this->data[curCap] == nullptr) {
          this->data[curCap] = system->getNewFirstIndirectBlock();
          if (this->data[curCap] == nullptr) {
              std::cerr << "Cannot allocate new FirstIndirectionBlock: Drive might be full!" << std::endl;
              return false;
          }
      }
    return this->data[curCap]->appendDataBlock(block, system);
  }
  return true;
}

bool SecondIndirectBlock::trimToSize(size_t newSize) {
    if (newSize > this->getByteCapacity())
            return false;
    size_t curCap = this->getLength();
    if (curCap < newSize)
        return false;
    size_t newBlockCount = newSize / BLOCK_SIZE + (newSize % BLOCK_SIZE == 0 ? 0 : 1);//If newSize is a bytecount, we maybe need to append one more
    if (newBlockCount > this->getCapacity())
        return false;
    for (size_t curBlock = newBlockCount; curBlock < curCap; curBlock++) {
        if (data[curBlock] != nullptr) {
            data[curBlock]->status = Status::FREE;
            data[curBlock] = nullptr;
        }
    }
    return true;
}

unsigned long long SecondIndirectBlock::getLength() {
    unsigned long long output = 0;
    for (size_t i = 0; i < this->BLOCK_SIZE; i++) {
        if (this->data[i] == nullptr) {
            return i;
        }
    }
    return this->BLOCK_SIZE;
}

bool ThirdIndirectBlock::setData(Array *data) {
  unsigned int maxSize = data->getLength();
  if (maxSize > this->getLength()) {
    std::cerr
        << "Cannot save data in this ThirdIndirectBlock: Capacity is too small!"
        << std::endl;
    return false;
  }
  if (maxSize == 0) {//To clear the data, we take a array with size 0
      for (unsigned long i = 0; i < BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE; i++) {
          this->data[i]->setData(&Array::EMPTY_ARRAY);
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
    if (!(this->data[i]->setData(&dataSlice))) {
      std::cerr << "Cannot save data in SecondIndirectBlock " << i << std::endl;
      return false;
    }
    i++;
    curSize -= blockLen;
    offset += blockLen;
  }
  return true;
}

Array ThirdIndirectBlock::getData() {
  unsigned long cap = getLength();
  unsigned long countBlocks = cap / BLOCK_SIZE;
  Array output = new Array(cap);
  char *curP = (char *)output.getArray();
  for (unsigned long i = 0; i < countBlocks; i++) {
    std::strncpy(curP, (const char *)this->data[i]->getData().getArray(),
                 BLOCK_SIZE);
    curP += BLOCK_SIZE;
  }

  return output;
}

bool ThirdIndirectBlock::appendDataBlock(DataBlock* block, INodeSystem* system) {
    unsigned long curCap = this->getLength();
      if (curCap >= this->getCapacity()) {
        return false;
      }
      if (this->data[curCap] == nullptr) {
          this->data[curCap] = system->getNewSecondIndirectBlock();
          if (this->data[curCap] == nullptr) {
              std::cerr << "Cannot allocate new SecondIndirectionBlock: Drive might be full!" << std::endl;
              return false;
          }
      }
      if (!this->data[curCap++]->appendDataBlock(block, system)) {// WARNING: Increases curCap after this call!
          if (this->data[curCap] == nullptr) {
              this->data[curCap] = system->getNewSecondIndirectBlock();
              if (this->data[curCap] == nullptr) {
                  std::cerr << "Cannot allocate new SecondIndirectionBlock: Drive might be full!" << std::endl;
                  return false;
              }
          }
        return this->data[curCap]->appendDataBlock(block, system);
      }
      return true;
}


bool ThirdIndirectBlock::trimToSize(size_t newSize) {

}

unsigned long long ThirdIndirectBlock::getLength() {
    unsigned long long output = 0;
    for (size_t i = 0; i < this->BLOCK_SIZE; i++) {
        if (this->data[i] == nullptr) {
            return i;
        }
    }
    return this->BLOCK_SIZE;
}
