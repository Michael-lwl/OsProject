#ifndef BS_FILE_SYSTEM
#define BS_FILE_SYSTEM

#include "./system.h"
#include "utils.h"
#include <cstddef>
#include <exception>

#define MAX_FILE_COUNT (static_cast<unsigned int>(-1))

class BsFile;
class BsFat;

struct BsCluster
{
    ///The next cluster in the associated file, might be null if this cluster is unused or the last cluster in a File
    struct BsCluster *next;
    ///The previous cluster in the associated file, might be null if this cluster is unused or the first in a File
    struct BsCluster *previous;
    ///The status of this cluster
    unsigned char status;
    ///The index of this cluster
    unsigned int blockIndex;
    ///The data asociated with this cluster
    unsigned char *data;
};

class BsFile : public File {
    public:

        BsFile(BsFat* fileSystem, std::string* filePath, unsigned char flags, unsigned long reservedSpaceInBytes): File(filePath, flags, reservedSpaceInBytes) {
            filesystem = fileSystem;

        }
        ///Creates an empty BsFile
        BsFile(BsFat* fileSystem, std::string* filePath, unsigned char flags): BsFile(fileSystem, filePath, flags, 0) {

        }

        ~BsFile() {
            freeFile();
            filesystem = nullptr;
            clusterCount = 0;
        }

        bool setData(Array* data);
        Array* getData();
        bool resizeFile(unsigned long newFileSize);

        BsCluster* getFileStart() {return fileStart;}
        BsFat* getFileSystem() {return filesystem;}
        unsigned int getClusterCount() {return clusterCount;}

    protected:
        bool trimToSize(unsigned long newFileSize);
        bool expandToSize(unsigned long newFileSize);

    private:

        /// @brief sets all used clusters to empty clusters (does not empty the data)
        void freeFile();
        void removeAllClusters(BsCluster* startCluster);
        BsFat* filesystem;
        ///The first cluster for this file
        struct BsCluster *fileStart;
        ///The count of clusters used by this file
        unsigned int clusterCount;

};

class BsFat : public System
{

    public:

        BsFat(unsigned long memorySize, unsigned long blockSize, Data* dataHandler) : System(dataHandler) {
            unsigned int blockCount = memorySize / blockSize;
            auto memory = new unsigned char[blockCount * (sizeof(BsCluster) + blockSize)];
            //auto* memory= (unsigned char *) malloc (blockCount * (sizeof(struct BsCluster) + blockSize));
            if (memory == nullptr){
                throw ("Cannot assign Memory!");
            }
            this->blocks = (BsCluster *) memory;
            this->blockCount = blockCount;
            this->blockSize = blockSize;
            this->dataPtr = (unsigned char*) this->blocks + this->blockCount * sizeof(struct BsCluster);

            for (unsigned int i = 0; i < blockCount; i++) {
                auto*cluster = (struct BsCluster *)(memory + i * sizeof(struct BsCluster));
                cluster->status = Status::FREE;
                cluster->blockIndex = i;
                cluster->previous = nullptr;
                cluster->next = nullptr;
                cluster->data = this->dataPtr + i * blockSize;
            }
        }

        ~BsFat() {
            for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
                delete files[i];
            }
            safeFree(blocks);
        }

        //Overridden function
        bool deleteFile(std::string* filePath);
        unsigned long getFreeSpace();
        unsigned long getFileCount();
        unsigned long getFileSize(std::string* filePath);
        File* createFile(std::string* filePath, unsigned long fileSize, unsigned char flags);
        File* getFile(std::string* filePath);
        unsigned long calcTotalSize(Array* data);

        //Fat functions

        ///returns false if the file array is full
        bool hasFreeFileSpace();
        /// @brief tries to get a free (random) cluster for the amount of blocks, after that it will return a nullptr
        BsCluster* getNewCluster();
        unsigned int getFirstFreeFileIndex();

        //getter and setter
        unsigned char* getDataPtr() {return dataPtr;}
        unsigned long getDataSize() {return blockSize * blockCount;}
        unsigned int getBlockSize() {return blockSize;}
        unsigned int getBlockCount() {return blockCount;}


    private:
        ///May return nullptr if there is no file with that name
        BsFile* getBsFileForPath(const std::string* path);

        BsFile* files[MAX_FILE_COUNT];
        /// The size of a block in bytes
        unsigned int blockSize;
        unsigned int blockCount;
        /// All blocks as one big array.
        ///WARNING: Its size is not "blocksize * sizeof(BsCluster)" but "blockCount * (sizeof(struct BsCluster) + blockSize)" because the data is appended (although similiarly sorted) after the Clusters
        BsCluster *blocks;
        ///The data (which is canonically right after the last Cluster) with the length of blockCount * blockSize
        ///Pls dont tamper with this data directly as it has to be at the same index as its paired cluster
        unsigned char* dataPtr;
};
#endif
