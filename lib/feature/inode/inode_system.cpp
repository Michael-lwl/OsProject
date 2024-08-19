#include "./../../../include/feature/inode/inode_system.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

/// Deletes the file with the specified path, and only the specified file
bool INodeSystem::deleteFile(std::string *filePath) {
  std::shared_ptr<File> mFile = getFile(filePath);
  if (mFile == nullptr) {
    std::cerr << "File \"" << filePath << "\" does not exist!" << std::endl;
    return false;
  }
  INode *file = static_cast<INode *>(mFile.get());
  file->resizeFile(0);
  file->setFlags(0);
  return true;
}

/// Returns the accessible free space of this partition
unsigned long INodeSystem::getFreeSpace() {
  size_t count = 0;
  for (size_t i = 0; i < this->iNodeCount; i++) {
    if (this->iNodes[i].getFileSizeInBytes() > 0) {
      count++;
    }
  }
  if (count == this->iNodeCount) {
    std::cerr << "Warning cannot use any more INodes!" << std::endl;
    return 0;
  }
  count = this->getDriveSize();
  for (size_t i = 0; i < this->iNodeCount; i++) {
    count -= this->iNodes[i].getFileSizeInBytes();
  }
  return count;
}

/// Returns the amount of stored files
unsigned long INodeSystem::getFileCount() {
  unsigned long count = 0;
  for (size_t i = 0; i < this->iNodeCount; i++) {
    if (this->iNodes[i].getFileSizeInBytes() > 0) {
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

  shared_ptr<INode> inode = nullptr;
  for (size_t i = 0; i < iNodeCount; i++) {
    if ((iNodes + i)->getFlags() == 0) {
      inode = make_shared<INode>(iNodes + i);
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

  return inode;
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
    std::static_pointer_cast<INode>(file)->setMtime(now_t_t);
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
    curFile = getChild(static_pointer_cast<Directory>(curFile), f);
    if (curFile == nullptr) {
      return nullptr;
    }
  }
  time_t now_t_t = getCurrentTime();
  static_pointer_cast<INode>(curFile)->setAtime(now_t_t);

  return curFile;
}

/// Returns the file with the INodes' id.
/// Might return null!
/// TODO: Fix the std::make_shared functions!
std::shared_ptr<File> INodeSystem::getFile(unsigned long iNodeId) {
  using namespace std;
  INode *inode = this->iNodes + iNodeId;
  shared_ptr<INode> file = make_shared<INode>(inode);
  if (!Directory::isDirectory(file.get())) {
    return static_pointer_cast<File>(file);
  }
  // This is probably useless, but we try to force the output to be a directory
  shared_ptr<INodeDirectory> inodeDir =
      static_pointer_cast<INodeDirectory>(file);
  shared_ptr<Directory> dir = static_pointer_cast<Directory>(inodeDir);
  return static_pointer_cast<File>(dir);
}

std::shared_ptr<File> INodeSystem::getRoot() {
  return getFile(static_cast<unsigned long>(0));
}

std::shared_ptr<File>
INodeSystem::getChild(std::shared_ptr<Directory> directory,
                      const std::string &fileName) {
  using namespace std;
  if (directory == nullptr || Directory::isDirectory(directory.get())) {
    cerr << "Error: The directory is invalid!" << endl;
    return nullptr;
  }
  if (fileName.empty()) {
    cerr << "Error: The Filename is empty!" << endl;
    return nullptr;
  }
  shared_ptr<INodeDirectory> iDir =
      static_pointer_cast<INodeDirectory>(directory);
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
