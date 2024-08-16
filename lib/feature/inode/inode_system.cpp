#include "./../../../include/feature/inode/inode_system.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

/// Deletes the file with the specified path, and only the specified file
bool INodeSystem::deleteFile(std::string *filePath) {
    std::shared_ptr<File> mFile = getFile(filePath);
    if (mFile == nullptr) {
        std::cerr<<"File \""<<filePath<<"\" does not exist!"<<std::endl;
        return false;
    }
    INode* file = static_cast<INode*>(mFile.get());
    file->resizeFile(0);
    file->setFlags(0);
    return true;
}

/// Returns the accessible free space of this partition
unsigned long INodeSystem::getFreeSpace() {
    size_t count = 0;
    for (size_t i = 0; i < this->iNodeCount; i++) {
        if (this->iNodes[i].getSize() > 0) {
            count++;
        }
    }
    if (count == this->iNodeCount) {
        std::cerr<<"Warning cannot use any more INodes!"<<std::endl;
        return 0;
    }
    count = this->getDriveSize();
    for (size_t i = 0; i < this->iNodeCount; i++) {
        count -= this->iNodes[i].getSize();
    }
    return count;
}

/// Returns the amount of stored files
unsigned long INodeSystem::getFileCount() {
    unsigned long count = 0;
    for (size_t i = 0; i < this->iNodeCount; i++) {
        if (this->iNodes[i].getSize() > 0) {
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
                                              unsigned char flags) {}

/// Saves the content in the specified file, might resize it, if the reserved
/// space is too small.
bool INodeSystem::saveInFile(std::string *filePath,
                             std::shared_ptr<Array> data) {
    auto file = getFile(filePath);
    if (file == nullptr) {
        return false;
    }
    return file->setData(data.get());
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
    // TODO: is this legal?
    curFile = getChild(make_shared<Directory>(static_cast<Directory *>(curFile.get())), f);
    if (curFile == nullptr) {
      return nullptr;
    }
  }

  return curFile;
}

/// Returns the file with the INodes' id.
/// Might return null!
std::shared_ptr<File> INodeSystem::getFile(unsigned long iNodeId) {
    return std::make_shared<File>(this->iNodes[iNodeId]);
}

std::shared_ptr<File> INodeSystem::getRoot() {
  return getFile((unsigned long)0);
}

std::shared_ptr<File>
INodeSystem::getChild(std::shared_ptr<Directory> directory,
                      const std::string &fileName) {
  if (directory == nullptr|| Directory::isDirectory(directory.get())||
      fileName.empty()) {
    return nullptr;
  }

  return //TODO!

}
