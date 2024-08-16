#include "./../../../include/feature/fat/bs_system.h"
#include <iostream>
#include <memory>

bool filePathIsBlank(const std::string* filePath) {
    return filePath == nullptr || filePath->empty();
}
///May return nullptr if there is no file with that name
std::shared_ptr<BsFile> BsFat::getBsFileForPath(const std::string* path) {
    if (filePathIsBlank(path)){
        return nullptr;
    }
    for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
        if ((*path).compare(*files[i]->getFilePath())){
            return files[i];
        }
    }
    return nullptr;
}

bool BsFat::deleteFile(std::string* filePath) {
    if (filePathIsBlank(filePath))
        return false;
    std::shared_ptr<BsFile> f = getBsFileForPath(filePath);
    if (f == nullptr)
        return false;
    f.reset();
    return true;
}

unsigned long BsFat::getFreeSpace() {
    if (!hasFreeFileSpace())
        return 0;
    auto counter = 0;
    for (unsigned int i = 0; i < blockCount; i++)
    {
        if (blocks[i]->status == Status::FREE)
            counter++;
    }
    return counter;
}

bool BsFat::hasFreeFileSpace()
{
    return getFileCount() != 0;
}

unsigned long BsFat::getFileCount() {
    unsigned long output = 0;
    for (unsigned long i = 0; i < MAX_FILE_COUNT; i++) {
        if (files[i]->getFileStart() != nullptr)
            output++;
    }
    return output;
}

unsigned long BsFat::getFileSize(std::string* filePath) {
    std::shared_ptr<BsFile> file = getBsFileForPath(filePath);
    if (file == nullptr) {
        std::cerr<<"Couldn't get file \""<<filePath<<"\""<<std::endl;
        return 0;
    }
    return file->getFileSizeInBytes();
}

std::shared_ptr<File> BsFat::createFile(std::string* filePath, unsigned long fileSize, unsigned char flags) {
    using namespace std;

    if (filePathIsBlank(filePath)) {
        cerr<<"Cannot create file \""<<filePath<<"\": Filepath cannot be empty!"<<endl;
        return nullptr;
    }

    if (getBsFileForPath(filePath) != nullptr) {
        cerr<<"Cannot create file \""<<filePath<<"\": File already exists!"<<endl;
        return nullptr;
    }

    int index = getFirstFreeFileIndex();
    shared_ptr<BsFile> f = make_shared<BsFile>(this, filePath, fileSize, flags);
    files[index] = f;
    return f;
}
bool BsFat::saveInFile(std::string* filePath, std::shared_ptr<Array> data){
    auto file = getFile(filePath);
    if (file == nullptr)
        return false;
    return file.get()->setData(data.get());
}

std::shared_ptr<File> BsFat::getFile(std::string* filePath) {
    return getBsFileForPath(filePath);
}

std::shared_ptr<BsCluster> BsFat::getNewCluster() {
    std::shared_ptr<BsCluster> output = nullptr;
    unsigned int initialOffset = rand() % blockCount;
    unsigned int offset = initialOffset;
    do
    {
        output = blocks[offset++];
        if (offset >= blockCount)
            offset = 0;
    } while (output->status != Status::FREE && offset != initialOffset);

    if (output->status != Status::FREE)
    {
        return nullptr;
    }
    return output;
}

unsigned int BsFat::getFirstFreeFileIndex() {
    for (unsigned int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (files[i] == nullptr || files[i]->getFileStart() == nullptr)
            return i;
    }
    return -1;
}
