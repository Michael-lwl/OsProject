#include "./../../../include/feature/fat/bs_system.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

char BsFat::getCharForObjective(BsCluster *cluster) {
    if (cluster == nullptr)
        return StatusChar::DEF_CHAR;
    char output = getCharForStatus(cluster->status);
    if (output == StatusChar::USED_CHAR)
    {
        for (unsigned int i = 0; i < MAX_FILE_COUNT; i++)
        {
            if (this->files[i] == nullptr || this->files[i].get() == nullptr)
                continue;
            if (this->files[i]->getFileStart() == cluster)
                output = i + '0';
        }
    }
    return output;
}

void BsFat::show() {
    std::cout << colorize("|", Color::WHITE);
    for (unsigned int i = 0; i < this->getBlockCount(); i++)
    {
        BsCluster* c = this->getCluster(i);
        std::string str{this->getCharForObjective(c)};
        std::cout << colorize(str, getColorForStatus(c->status));
        std::cout << '|';
    }
    std::cout<<std::endl;
}


bool filePathIsBlank(const std::string* filePath) {
    return filePath == nullptr || filePath->empty();
}
///May return nullptr if there is no file with that name
std::shared_ptr<BsFile> BsFat::getBsFileForPath(const std::string* path) {
    if (filePathIsBlank(path)){
        return nullptr;
    }
    for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
        if (files[i] == nullptr || files[i]->getFilePath() == nullptr){
            continue;
        }
        if ((*path).compare(*files[i]->getFilePath()) == 0){
            return files[i];
        }
    }
    return nullptr;
}

long BsFat::getBsFileIndexForPath(const std::string* path) {
    if (filePathIsBlank(path)){
        return -1;
    }
    for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
        if (files[i]->getFilePath() == nullptr){
            continue;
        }
        if ((*path).compare(*files[i]->getFilePath()) == 0){
            return i;
        }
    }
    return -1;
}

bool BsFat::deleteFile(std::string* filePath) {
    if (filePathIsBlank(filePath))
        return false;
    auto f = getBsFileForPath(filePath);
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
        if (getCluster(i)->status == Status::FREE)
            counter++;
    }
    return counter * this->BLOCK_SIZE;
}

bool BsFat::hasFreeFileSpace()
{
    return getFileCount() != 0;
}

unsigned long BsFat::getFileCount() {
    unsigned long output = 0;
    for (unsigned long i = 0; i < MAX_FILE_COUNT; i++) {
        if (files[i]->getFlags() != 0)
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
    if (index == -1) {
        return nullptr;
    }
    files[index] = make_shared<BsFile>(this, filePath, flags, fileSize);
    return files[index];
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

std::vector<std::shared_ptr<File>> BsFat::getAllFiles() {
    std::vector<std::shared_ptr<File>> output;

    for (size_t i = 0; i < MAX_FILE_COUNT; i++) {
        if (this->files[i]->getFlags() != 0) {
            output.push_back(this->files[i]);
        }
    }

    return output;
}

BsCluster* BsFat::getNewCluster() {
    BsCluster* output = nullptr;
    unsigned int initialOffset = rand() % blockCount;
    unsigned int offset = initialOffset;
    do
    {
        output = getCluster(offset++);
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
    for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
        if (files[i] == nullptr)
            return i;
        if (files[i]->getFlags() == 0)
            return i;
        if (files[i]->getFileStart() == nullptr)
            return i;
    }
    return -1;
}

float BsFat::getFragmentation()
{
    long free = 0;
    long freeMax = 0;
    long counter = 0;

    for (unsigned int i = 0; i < this->blockCount; i++)
    {
        if (this->getCluster(i)->status == Status::FREE)
        {
            free++;
            counter++;
            continue;
        }
        if (freeMax < counter)
        {
            freeMax = counter;
        }
        counter = 0;
    }
    if (free == 0)
        return 0.0;
    return ((float)freeMax / free);
}

bool BsFat::defragDisk() {
    using namespace std;
    showDefragMsg(0);
    vector<vector<BsCluster*>> usedFiles;
    ///vector of all data from all files
    vector<vector<vector<char>>> usedData;

    // Collect all used clusters
    for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
        if (this->files[i] == nullptr)
            continue;
        BsFile* file = this->files[i].get();
        if (file == nullptr || file->getFileStart() == nullptr)
            continue;
        BsCluster* curCluster = file->getFileStart();

        vector<BsCluster*> curFile;
        vector<vector<char>> curFileData;
        do {
            curFile.push_back(curCluster);
            vector<char> curData;
            curData.reserve(BLOCK_SIZE);
            unsigned char* dataBlock = getDataBlock(curCluster->blockIndex);
            if (dataBlock) {
                memcpy(curData.data(), dataBlock, BLOCK_SIZE);
            }
            curFileData.push_back(curData);
            curCluster = curCluster->next;
        } while (curCluster != nullptr);
        usedFiles.push_back(curFile);
        usedData.push_back(curFileData);
    }
    showDefragMsg(25);

    // Rearranging clusters
    unsigned int blockCounter = 0;
    unsigned int newIndex = 0;
    char isFilestart = 0;
    vector<BsCluster*> fileStarts;
    for (vector<BsCluster*> file : usedFiles)
    {
        isFilestart = 1;
        for (BsCluster* cluster : file)
        {
            if (newIndex >= this->blockCount) {
                std::cerr<<"Error: newIndex is too high"<<std::endl;
                break;
            }
            while (this->getCluster(newIndex)->status == Status::CORRUPTED || this->getCluster(newIndex)->status == Status::RESERVED)
            {
                newIndex++;
                if (newIndex >= this->blockCount){
                    cerr<<"Everything is corrupted or reserved!"<<endl;
                    return false;
                }
            }
            cluster->blockIndex = newIndex;
            cluster->data = this->getDataBlock(blockCounter);
            if (isFilestart == 0)
            {
                this->getCluster(newIndex - 1)->next = this->getCluster(newIndex);
                this->getCluster(newIndex)->previous = this->getCluster(newIndex - 1);
            }
            else
            {
                this->getCluster(newIndex)->previous = nullptr;
                fileStarts.push_back(this->getCluster(newIndex));
                isFilestart = 0;
            }
            this->getCluster(newIndex)->blockIndex = cluster->blockIndex;
            this->getCluster(newIndex)->data = cluster->data;
            this->getCluster(newIndex)->next = cluster->next;
            this->getCluster(newIndex)->previous = cluster->previous;
            this->getCluster(newIndex)->status = cluster->status;
            newIndex++;
            blockCounter++;
        }
    }
    //Rearrange data
    blockCounter = 0;
    for (auto curFileData: usedData){
        ///curFileData: all data from one file
        for (auto curData: curFileData){
            //curData: all data from one block
            while (this->getCluster(blockCounter)->status == Status::CORRUPTED || this->getCluster(blockCounter)->status == Status::RESERVED)
            {
                blockCounter++;
                if (blockCounter >= this->blockCount){
                    cerr<<"Everything is corrupted or reserved!"<<endl;
                    return false;
                }
            }
            unsigned int dataCounter = 0;
            for (char d : curData) {
                unsigned char* index = this->getDataBlock(dataCounter);
                *index = d;
                dataCounter++;
            }
            //map the data to the current Cluster
            this->getCluster(blockCounter)->data = this->getDataBlock(blockCounter);
        }
        blockCounter++;
    }

    showDefragMsg(50);
    // Mark the remaining clusters as free
    for (unsigned int i = newIndex; i < this->blockCount; i++)
    {
        if (this->getCluster(i)->status == Status::CORRUPTED || this->getCluster(i)->status == Status::RESERVED){
            continue;
        }
        this->getCluster(i)->status = Status::FREE;
        this->getCluster(i)->next = nullptr;
        this->getCluster(i)->previous = nullptr;
    }
    showDefragMsg(75);

    // Set the filestarts
    unsigned int counter = 0;
    for (unsigned int i = 0; i < MAX_FILE_COUNT && counter < fileStarts.size(); i++)
    {
        if (this->files[i]->getFileStart() != nullptr)
        {
            this->files[i]->setFileStart(fileStarts.at(counter++));
        }
    }
    showDefragMsg(100);
    return true;
}
