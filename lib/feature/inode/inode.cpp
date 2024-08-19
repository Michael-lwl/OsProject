#include "./../../../include/feature/inode/inode.h"
#include <iostream>
#include <memory>

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
  size_t interval = DataBlock::getBlockSize();
  size_t counter;
  for (counter = 0; counter < dataLen && counter < DIRECT_DATA_BLOCK_COUNT;
       counter += interval) {
    Array curData = new Array(interval, data->getArray() + counter,
                              MemAllocation::DONT_DELETE);
    this->datablocks[counter]->setData(&curData);
  }

  size_t remainingDataLen = dataLen - counter;
  if (remainingDataLen == 0) {//if the remainder is 0, we are finished!
      return true;
  }

  // TODO: set data in indirect blocks

  return false;
}

std::unique_ptr<Array> getData() {}

bool resizeFile(unsigned long newFileSize) {}

std::vector<INode *> INodeDirectory::getChildren() {}

bool INodeDirectory::addChild(std::shared_ptr<File> file) {}

bool INodeDirectory::removeChild(std::string filename) {}

bool INodeDirectory::removeChild(size_t childId) {}
