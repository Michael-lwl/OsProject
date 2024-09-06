#include "./../../../include/feature/inode/inode_system.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

/// Deletes the file with the specified path, and only the specified file
bool INodeSystem::deleteFile(std::string *filePath) {
  INode* file = findFile(filePath);
  if (file == nullptr) {
    std::cerr << "File \"" << filePath << "\" does not exist!" << std::endl;
    return false;
  }
  *SysOut() << "Deleting File \"" << *filePath << "\"." << std::endl;
  file->resizeFile(0);
  file->setFlags(0);
  file->rename(nullptr);
  new (file) INode(nullptr, 0, 0, this);
  return true;
}

/// Returns the accessible free space of this partition
unsigned long INodeSystem::getFreeSpace() {
  size_t count = 0;
  for (size_t i = 0; i < this->iNodeCount; i++) {
    if (this->getINode(i)->getFileSizeInBytes() > 0) {
      count++;
    }
  }
  if (count == this->iNodeCount) {
    std::cerr << "Warning cannot use any more INodes!" << std::endl;
    return 0;
  }
  count = this->getDriveSize();
  for (size_t i = 0; i < this->iNodeCount; i++) {
    count -= this->getINode(i)->getFileSizeInBytes();
  }
  return count;
}

/// Returns the amount of stored files
unsigned long INodeSystem::getFileCount() {
  unsigned long count = 0;
  for (size_t i = 0; i < this->iNodeCount; i++) {
    if (this->getINode(i)->getFileSizeInBytes() > 0) {
      count++;
    }
  }
  return count;
}

/// Returns the size of the specified file
unsigned long INodeSystem::getFileSize(std::string *filePath) {
  using namespace std;

  shared_ptr<File> f = getFile(filePath);
  if (f == nullptr) {
    cerr << "Cannot find file size for file \"" << filePath
         << "\"! File does not exist" << endl;
    return 0;
  }

  return f->getFileSizeInBytes();
}

/// Creates a file on the specified path. It sets the flags and reserves the
/// specified space. (If you want to save the content, you would have to save it
/// via saveInFile afterwards) Generally strifes to be at least done in O(n)
std::shared_ptr<File> INodeSystem::createFile(std::string *filePath,
                                              unsigned long fileSize,
                                              unsigned char flags) {
  using namespace std;
  if (filePath == nullptr || fileSize == 0 || flags == 0) {
    cerr << "Error: cannot create File, wrong parameters!" << endl;
    return nullptr;
  }

  INode *inode = nullptr;
  for (size_t i = 0; i < iNodeCount; i++) {
    if (getINode(i)->getFlags() == 0) {
      inode = getINode(i);
      break;
    }
  }
  if (inode == nullptr) {
    cerr << "Error: Could not create File! No available INodes!" << endl;
    return nullptr;
  }

  time_t now_t_t = getCurrentTime();
  inode->setMtime(now_t_t);
  inode->setCtime(now_t_t);
  inode->setAtime(now_t_t);

  inode->rename(filePath);
  inode->resizeFile(fileSize);
  inode->setFlags(flags);

  return std::make_shared<INode>(inode);
}

/// Saves the content in the specified file, might resize it, if the reserved
/// space is too small.
bool INodeSystem::saveInFile(std::string *filePath,
                             std::shared_ptr<Array> data) {
  std::shared_ptr<File> file = getFile(filePath);
  if (file == nullptr) {
    return false;
  }
  bool output = file->setData(data.get());
  if (output) {
    time_t now_t_t = getCurrentTime();
    std::dynamic_pointer_cast<INode>(file)->setMtime(now_t_t);
  }
  return output;
}

/// Returns a the file associated with the specified path.
/// Might return null!
std::shared_ptr<File> INodeSystem::getFile(std::string *filePath) {
  using namespace std;
  INode* curFile = findFile(filePath);

  time_t now_t_t = getCurrentTime();
  curFile->setAtime(now_t_t);

  return std::make_shared<INode>(curFile);
}

INode* INodeSystem::findFile(std::string* filePath) {
    using namespace std;
    if (filePath == nullptr || filePath->empty()) {
      cerr << "Cannot find file: Filepath is empty or null!" << endl;
      return nullptr;
    }
    vector<string> foldersNFile = splitAt(filePath, '/');
    shared_ptr<File> curFile;

    if (foldersNFile.size() > 1) {///currently no directory support
      curFile = getRoot();
      for (const string &f : foldersNFile) {
        if (!Directory::isDirectory(curFile.get())) {
          return nullptr;
        }
        Directory *dir = static_cast<INodeDirectory *>(static_cast<INode *>(curFile.get()));
        curFile = getChild(dir, f);
        if (curFile == nullptr) {
          return nullptr;
        }
      }
    } else {
      for (size_t i = 0; i < iNodeCount; i++) {
        if (this->getINode(i)->getFlags() != 0 && this->getINode(i)->getFilePath()->compare(*filePath)) {
          return this->getINode(i);
        }
      }
    }
    return nullptr;
}

/// Returns the file with the INodes' id.
/// Might return null!
std::shared_ptr<File> INodeSystem::getFile(unsigned long iNodeId) {
  using namespace std;
  if (iNodeId > iNodeCount) {
    return nullptr;
  }
  INode *file = getINode(iNodeId);
  if (!Directory::isDirectory(file)) {
    return std::static_pointer_cast<File>(std::make_shared<INode>(file));
  }
  // This is probably useless, but we try to force the output to be a directory
  shared_ptr<INodeDirectory> inodeDir =
      static_pointer_cast<INodeDirectory>(std::make_shared<INode>(file));
  shared_ptr<Directory> dir = static_pointer_cast<Directory>(inodeDir);
  return static_pointer_cast<File>(dir);
}

float INodeSystem::getFragmentation() {
  long free = 0;
  long freeMax = 0;
  long counter = 0;

  for (unsigned int i = 0; i < this->dataBlockCount; i++) {
    if (this->getDataBlock(i)->status == Status::FREE) {
      free++;
      counter++;
      continue;
    }
    if (freeMax < counter)
      freeMax = counter;
    counter = 0;
  }
  if (free == 0)
    return 0.0;
  return ((float)freeMax / free);
}

bool INodeSystem::defragDisk() {
  using namespace std;
  // Temp struct to keep in a single vector
  typedef struct TempINode {
    INode inode;
    std::shared_ptr<Array> data;

    TempINode(TempINode* tmpNode): inode(tmpNode->inode), data(tmpNode->data) {}
    TempINode(INode inode, std::unique_ptr<Array> data): inode(inode), data(std::move(data)) {}

  } TempINode;
  const size_t DATA_BLOCK_BLOCK_SIZE = BLOCK_SIZE - sizeof(DataBlock);

  showDefragMsg(0);
  vector<TempINode>* usedFiles = new vector<TempINode>();
  usedFiles->reserve(this->iNodeCount / 2);
  // Collect all used files
  for (unsigned int i = 0; i < iNodeCount; i++) {
    INode file = this->iNodes[i];
      if (file.getFilePath() == nullptr || file.getFlags() == 0 || file.getFileSizeInBytes() == 0)
        continue;
      std::unique_ptr<Array> data = file.getData();
      TempINode tmp = {&file, std::move(data)};
      usedFiles->push_back(tmp);
  }
  showDefragMsg(25);
  // Clearing INodes
  for (size_t i = 0; i < iNodeCount; i++) {
      new (getINode(i)) INode(nullptr, 0, 0, this);
  }
  showDefragMsg(50);
  // Clearing data!
  for (size_t i = 0; i < dataBlockCount; i++) {
      if (getDataBlock(i) != nullptr && getDataBlock(i)->status != RESERVED)
        new (getDataBlock(i)) DataBlock(DATA_BLOCK_BLOCK_SIZE);
  }
  showDefragMsg(75);
  // Setting all data
  size_t curINode = 0;
  size_t curBlock = 0;
  for (TempINode tmp : *usedFiles) {
    /// set all blocks
    size_t len = tmp.data->getLength();
    size_t fullBlocks = len / DATA_BLOCK_BLOCK_SIZE;
    size_t remainder = len % DATA_BLOCK_BLOCK_SIZE;
    size_t totalBlocks = fullBlocks + (remainder > 0 ? 1 : 0);
    unsigned char* dataPtr = tmp.data->getArray();
    /// create Inode with size 0
    INode* node = new (getINode(curINode)) INode(tmp.inode.getFilePath(), tmp.inode.getFlags(), 0, this);
    ///Create the datablocks and append them to inode
    DataBlock* db = nullptr;
    Array* curData = nullptr;
    for (size_t i = 0; i < totalBlocks && curBlock < this->dataBlockCount; i++){
        // if (curBlock > this->dataBlockCount) {
        //     cerr << "Filesystem is corrupt! Cannot find unused Blocks! Needs to be formatted!" << endl;
        //     return false;
        // }
        do {
            db = getDataBlock(curBlock++);
        } while ((db == nullptr || db->status == Status::RESERVED) && curBlock < this->dataBlockCount);
        if (db == nullptr) {
            cerr << "Filesystem is corrupt! Cannot find unused Blocks! Needs to be formatted!" << endl;
            return false;
        }
        size_t ARR_LEN = DATA_BLOCK_BLOCK_SIZE;
        if (dataPtr + ARR_LEN > tmp.data->getLength() + tmp.data->getArray()) {
            ARR_LEN = dataPtr - tmp.data->getArray();
        }
        new (db) DataBlock(DATA_BLOCK_BLOCK_SIZE);
        curData = new Array(ARR_LEN, dataPtr, MemAllocation::DONT_DELETE);
        db->setData(curData);
        db->status = Status::USED;
        dataPtr += ARR_LEN;
        node->appendDataBlock(db);
    }
  }

  showDefragMsg(100);
  return true;
}

DataBlock *INodeSystem::getNewDataBlock(unsigned char status) {
  DataBlock *output = nullptr;
  unsigned int initialOffset = rand() % dataBlockCount;
  unsigned int offset = initialOffset;
  do {
    output = getDataBlock(offset++);
    if (offset >= dataBlockCount)
      offset = 0;
  } while (output->status != Status::FREE && offset != initialOffset);

  if (output->status != Status::FREE) {
    return nullptr;
  }
  output->status = status;
  return output;
}

FirstIndirectBlock* INodeSystem::getNewFirstIndirectBlock() {
  DataBlock* output = getNewDataBlock(AdditionalStats::INDIRECT_1);
  if (output == nullptr) {
    return nullptr;
  }
  return new (output) FirstIndirectBlock(BLOCK_SIZE - sizeof(FirstIndirectBlock));
}

SecondIndirectBlock* INodeSystem::getNewSecondIndirectBlock() {
  DataBlock* output = getNewDataBlock(AdditionalStats::INDIRECT_2);
  if (output == nullptr) {
    return nullptr;
  }
  return new (output) SecondIndirectBlock(BLOCK_SIZE - sizeof(SecondIndirectBlock));
}
ThirdIndirectBlock* INodeSystem::getNewThirdIndirectBlock() {
  DataBlock* output = getNewDataBlock(AdditionalStats::INDIRECT_3);
  if (output == nullptr) {
    return nullptr;
  }
  return new (output) ThirdIndirectBlock(BLOCK_SIZE - sizeof(ThirdIndirectBlock));
}

std::shared_ptr<File> INodeSystem::getRoot() {
  return getFile(static_cast<unsigned long>(0));
}

std::shared_ptr<File> INodeSystem::getChild(Directory *directory,
                                            const std::string &fileName) {
  using namespace std;
  if (directory == nullptr || Directory::isDirectory(directory)) {
    cerr << "Error: The directory is invalid!" << endl;
    return nullptr;
  }
  if (fileName.empty()) {
    cerr << "Error: The Filename is empty!" << endl;
    return nullptr;
  }
  INodeDirectory *iDir = static_cast<INodeDirectory *>(directory);
  vector<INode *> iDirChildren = iDir->getChildren();
  for (auto inode : iDirChildren) {
    if (inode->getFilePath() == nullptr || inode->getFilePath()->empty()) {
      continue;
    }
    if (inode->getFilePath()->compare(fileName)) {
      shared_ptr<INode> inode_shared_ptr = make_shared<INode>(inode);
      return static_pointer_cast<File>(inode_shared_ptr);
    }
  }
  cerr << "The Directory does not contain the File \"" << fileName << "\"!"
       << endl;
  return nullptr;
}

char INodeSystem::getCharForObjective(DataBlock *db) {
  char output = getCharForStatus(db->status);
  if (output == StatusChar::USED_CHAR) {
    for (unsigned int i = 0; i < iNodeCount; i++) {
      if (this->getINode(i) != nullptr && this->getINode(i)->getDatablocks()[0] == db)
        output = i + '0';
    }
  }
  if (output == StatusChar::DEF_CHAR) {
    switch (db->status) {
      case AdditionalStats::INDIRECT_1:
        output = AdditionalStatsChars::INDIRECT_1_CHAR;
        break;
      case AdditionalStats::INDIRECT_2:
        output = AdditionalStatsChars::INDIRECT_2_CHAR;
        break;
      case AdditionalStats::INDIRECT_3:
        output = AdditionalStatsChars::INDIRECT_3_CHAR;
        break;
    }
  }
  return output;
}

void INodeSystem::show() {
  *SysOut() << colorize("|", Color::WHITE);
  for (unsigned int i = 0; i < this->dataBlockCount; i++) {
    DataBlock *c = this->getDataBlock(i);
    std::string str{this->getCharForObjective(c)};
    *SysOut() << colorize(str, getColorForStatus(c->status));
    *SysOut() << '|';
  }
  *SysOut() << std::endl;
}
