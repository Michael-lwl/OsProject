#include "bs_system.h"
#include <iostream>

bool filePathIsBlank(const std::string* filePath) {
    return filePath == nullptr || filePath->empty();
}
///May return nullptr if there is no file with that name
BsFile* BsFat::getBsFileForPath(const std::string* path) {
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
    BsFile* f = getBsFileForPath(filePath);
    if (f == nullptr)
        return false;
    delete f;
    return true;
}

unsigned long BsFat::getFreeSpace() {
    if (!hasFreeFileSpace())
        return 0;
    auto counter = 0;
    for (unsigned int i = 0; i < blockCount; i++)
    {
        if (blocks[i].status == Status::FREE)
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
    BsFile* file = getBsFileForPath(filePath);
    if (file == nullptr) {
        std::cerr<<"Couldn't get file \""<<filePath<<"\""<<std::endl;
        return 0;
    }
    return file->getFileSizeInBytes();
}

File* BsFat::createFile(std::string* filePath, unsigned long fileSize, unsigned char flags) {
    if (filePathIsBlank(filePath)) {
        std::cerr<<"Cannot create file \""<<filePath<<"\": Filepath cannot be empty!"<<std::endl;
        return nullptr;
    }

    if (getBsFileForPath(filePath) != nullptr) {
        std::cerr<<"Cannot create file \""<<filePath<<"\": File already exists!"<<std::endl;
        return nullptr;
    }

    int index = getFirstFreeFileIndex();
    BsFile* f = new BsFile(this, filePath, fileSize, flags);
    files[index] = f;
    return f;
}

File* BsFat::getFile(std::string* filePath) {
    return getBsFileForPath(filePath);
}

BsCluster* BsFat::getNewCluster() {
    struct BsCluster *output = nullptr;
    unsigned int initialOffset = rand() % blockCount;
    unsigned int offset = initialOffset;
    do
    {
        output = &blocks[offset++];
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

unsigned long BsFat::calcTotalSize(Array* data) {
    unsigned long output = 0;
    auto dataLength = getData()->getDataLength();
    auto totalLength = getData()->getTotalLength();
    for (unsigned long dataLen = data->getLength(); dataLen > 0; dataLen -= dataLength) {
        output += totalLength;
    }
    return output;

}
