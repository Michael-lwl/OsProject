#include "./../../../include/feature/inode/inode_system.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>
#include <vector>

/// Deletes the file with the specified path, and only the specified file
bool INodeSystem::deleteFile(std::string *filePath) {
  std::shared_ptr<File> mFile = getFile(filePath);
  if (mFile == nullptr) {
    std::cerr << "File \"" << filePath << "\" does not exist!" << std::endl;
    return false;
  }
  INode *file = dynamic_cast<INode *>(mFile.get());
  file->resizeFile(0);
  file->setFlags(0);
  return true;
}

/// Returns the accessible free space of this partition
unsigned long INodeSystem::getFreeSpace() {
  size_t count = 0;
  for (size_t i = 0; i < this->iNodeCount; i++) {
    if (this->iNodes[i]->getFileSizeInBytes() > 0) {
      count++;
    }
  }
  if (count == this->iNodeCount) {
    std::cerr << "Warning cannot use any more INodes!" << std::endl;
    return 0;
  }
  count = this->getDriveSize();
  for (size_t i = 0; i < this->iNodeCount; i++) {
    count -= this->iNodes[i]->getFileSizeInBytes();
  }
  return count;
}

/// Returns the amount of stored files
unsigned long INodeSystem::getFileCount() {
  unsigned long count = 0;
  for (size_t i = 0; i < this->iNodeCount; i++) {
    if (this->iNodes[i]->getFileSizeInBytes() > 0) {
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
    if (iNodes[i]->getFlags() == 0) {
      inode = iNodes[i];
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
  if (filePath == nullptr || filePath->empty()) {
    cerr << "Cannot find file: Filepath is empty or null!" << endl;
    return nullptr;
  }
  vector<string> foldersNFile;
  stringstream ss(*filePath);
  string segment;

  while (getline(ss, segment, '/')) {
    foldersNFile.push_back(segment);
  }

  shared_ptr<File> curFile = getRoot();
  for (const string &f : foldersNFile) {
    if (!Directory::isDirectory(curFile.get())) {
      return nullptr;
    }
    // TODO: is this legal
    Directory *dir =
        static_cast<INodeDirectory *>(static_cast<INode *>(curFile.get()));
    curFile = getChild(dir, f);
    if (curFile == nullptr) {
      return nullptr;
    }
  }
  time_t now_t_t = getCurrentTime();
  dynamic_pointer_cast<INode>(curFile)->setAtime(now_t_t);

  return curFile;
}

/// Returns the file with the INodes' id.
/// Might return null!
std::shared_ptr<File> INodeSystem::getFile(unsigned long iNodeId) {
  using namespace std;
  if (iNodeId > iNodeCount) {
    return nullptr;
  }
  INode *file = this->iNodes[iNodeId];
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
  // TODO: Implement the function!
  return 0.0f;
}

bool INodeSystem::defragDisk() {
  // TODO: Implement the function!
  return true;
}

DataBlock *INodeSystem::getNewDataBlock() {
  /// TODO: Maybe remove status and revisit this function
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
  return output;
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
      if (iNodes[i]->getDatablocks()[0] == db)
        output = i + '0';
    }
  } else if (output == StatusChar::DEF_CHAR) {
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
  std::cout << colorize("|", Color::WHITE);
  for (unsigned int i = 0; i < this->dataBlockCount; i++) {
    DataBlock *c = this->getDataBlock(i);
    std::string str{this->getCharForObjective(c)};
    std::cout << colorize(str, getColorForStatus(c->status));
    std::cout << '|';
  }
  std::cout << std::endl;
}
