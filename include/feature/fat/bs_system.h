#ifndef BS_FILE_SYSTEM
#define BS_FILE_SYSTEM

#include "./../../core/system.h"
#include "./../../utils.h"
#include <cstddef>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

//#define MAX_FILE_COUNT (static_cast<unsigned int>(-1))
#define MAX_FILE_COUNT 12
class BsFile;

struct BsCluster
{
    ///The next cluster in the associated file, might be null if this cluster is unused or the last cluster in a File
    BsCluster* next;
    ///The previous cluster in the associated file, might be null if this cluster is unused or the first in a File
    BsCluster* previous;
    ///The status of this cluster
    unsigned char status;
    ///The index of this cluster
    unsigned int blockIndex;
    ///The data asociated with this cluster
    unsigned char *data;
};

class BsFat : public System
{

    public:
        // Factory function to allocate and construct BsFat with extra memory
        static BsFat* create(void* memory, unsigned long driveSize, BlockSizes blockSize, Data* dataHandler) {
            size_t bsFatSize = sizeof(BsFat);
            if (driveSize <= bsFatSize) {
                return nullptr;
            }

            // Calculate the remaining space for BsCluster objects and data blocks
            unsigned long remainingSpace = driveSize - bsFatSize;
            size_t usedSpacePerBlock = sizeof(BsCluster) + blockSize;
            unsigned long blockCount = remainingSpace / usedSpacePerBlock;
            // If we cannot allocate any blocks, why should we allocate at all?
            if (blockCount == 0) {
                return nullptr;
            }
            // Totalsize is :  (BsFat + all clusters & data blocks)
            size_t totalSize = bsFatSize + blockCount * usedSpacePerBlock;

            if (totalSize > driveSize) {
                std::cerr << "Error in calculating BlockCount! Cannot create BsFat!" << std::endl;
                return nullptr;
            }

            // Use placement new to construct the BsFat object in the allocated memory
            BsFat* bsFat = new (memory) BsFat(driveSize, blockSize, dataHandler, blockCount);

            return bsFat;
        }

        char getCharForObjective(BsCluster *cluster);

        void show() override;

        ~BsFat() {
            for (size_t i = 0; i < MAX_FILE_COUNT; i++) {
                files[i].reset();
            }

        }

        void delFile(long index);
        void setBlocks(std::shared_ptr<BsFile> newFile);

        //Overridden function
        bool boot() override {return false;}
        bool deleteFile(std::string* filePath) override;
        unsigned long getFreeSpace() override;
        unsigned long getFileCount() override;
        unsigned long getFileSize(std::string* filePath) override;
        std::shared_ptr<File> createFile(std::string* filePath, unsigned long fileSize, unsigned char flags) override;
        bool saveInFile(std::string* filePath, std::shared_ptr<Array> data) override;
        std::shared_ptr<File> getFile(std::string* filePath) override;
        float getFragmentation() override;
        bool defragDisk() override;

        //Fat functions

        ///returns false if the file array is full
        bool hasFreeFileSpace();
        /// @brief tries to get a free (random) cluster for the amount of blocks, after that it will return a nullptr
        BsCluster* getNewCluster();
        unsigned int getFirstFreeFileIndex();

        //getter and setter
        unsigned long getDataSize() const {return BLOCK_SIZE * blockCount;}
        unsigned int getBlockCount() const {return blockCount;}

    protected:

    // Getter for a specific BsCluster object by index
    BsCluster* getCluster(size_t blockIndex) {
        if (blockIndex >= blockCount) {
            std::string bIStr = std::to_string(blockIndex);
            throw std::out_of_range(std::string("ClusterIndex ").append(bIStr).append(" out of bounds"));
        }
        return reinterpret_cast<BsCluster*>(reinterpret_cast<unsigned char*>(this) + sizeof(BsFat) + blockIndex * sizeof(BsCluster));
    }

    // Getter for the data block associated with a specific BsCluster
    unsigned char* getDataBlock(size_t blockIndex) {
        if (blockIndex >= blockCount) {
            std::string bIStr = std::to_string(blockIndex);
            throw std::out_of_range(std::string("DataBlockIndex ").append(bIStr).append(" out of bounds"));
        }
        return reinterpret_cast<unsigned char*>(this) + sizeof(BsFat) + (blockCount * sizeof(BsCluster)) + (blockIndex * BLOCK_SIZE);
    }

    private:
        // Private constructor to be used by the factory function
        BsFat(unsigned long driveSize, BlockSizes blockSize, Data* dataHandler, unsigned long blockCount)
            : System(dataHandler, driveSize, blockSize), blockCount(blockCount) {
            // Initialize BsClusters and associate data with each cluster
            for (size_t i = 0; i < blockCount; i++) {
                BsCluster* cluster = getCluster(i);
                new (cluster) BsCluster(); // Placement new to construct the cluster
                // Initialize the cluster fields
                cluster->status = Status::FREE;
                cluster->blockIndex = i;
                cluster->previous = nullptr;
                cluster->next = nullptr;
                cluster->data = getDataBlock(i);
            }
            for (size_t i = 0; i < MAX_FILE_COUNT; i++) {
                files[i] = std::make_shared<BsFile>(this, nullptr, 0, 0);
            }
        }
        ///May return nullptr if there is no file with that name
        std::shared_ptr<BsFile> getBsFileForPath(const std::string* path);
        long getBsFileIndexForPath(const std::string* path);

        unsigned int blockCount;
        std::shared_ptr<BsFile> files[MAX_FILE_COUNT];
};

class BsFile : public virtual File {
    public:

        BsFile(BsFat* fileSystem, std::string* filePath, unsigned char flags, unsigned long reservedSpaceInBytes):
                File(filePath, flags, reservedSpaceInBytes) {
            fileSizeInBytes = reservedSpaceInBytes;
            filesystem = fileSystem;
            if (reservedSpaceInBytes == 0) return;
            fileStart = fileSystem->getNewCluster();
            BsCluster* curCluster = fileStart;

            unsigned long plusOne = (reservedSpaceInBytes % filesystem->BLOCK_SIZE != 0 ? 1 : 0);
            clusterCount = (reservedSpaceInBytes / (filesystem->BLOCK_SIZE)) + (plusOne);

            BsCluster* next;

            for (unsigned long i = 1; i <= clusterCount; i++) {
                curCluster->status = Status::USED;
                next = filesystem->getNewCluster();
                if (next == nullptr) {
                    std::cerr<<"Couldnt allocate memory ("<<i<<"|"<<clusterCount<<")"<< std::endl;
                    throw new std::exception();
                }
                curCluster->next = next;
                next->previous = curCluster;
                curCluster = next;
                next->status = Status::USED;
            }
            curCluster->status = Status::USED;
        }
        ///Creates an empty BsFile (Reserves 0 Bytes of spacec)
        BsFile(BsFat* fileSystem, std::string* filePath, unsigned char flags): BsFile(fileSystem, filePath, flags, 0) {
            trimToSize(0);
        }

        ~BsFile() = default;

        bool setData(Array* data) override;
        std::unique_ptr<Array> getData() override;

        BsCluster* getFileStart() const {return fileStart;}
        void setFileStart(BsCluster* start) {fileStart = start;}
        BsFat* getFileSystem() const {return filesystem;}
        unsigned int getClusterCount() const {return clusterCount;}

        unsigned long getFileSizeInBytes() override {return fileSizeInBytes;}

    protected:
        bool trimToSize(unsigned long newFileSize) override;
        bool expandToSize(unsigned long newFileSize) override;
        void setFileSizeInBytes(size_t newFileSize) override {fileSizeInBytes = newFileSize;};

    private:

        ///Sets all used clusters to empty clusters (does not empty the data)
        void freeFile();
        void removeAllClusters(BsCluster* startCluster);
        unsigned int fileSizeInBytes;
        BsFat* filesystem;
        ///The first cluster for this file
        BsCluster* fileStart;
        ///The count of clusters used by this file
        unsigned int clusterCount;

};

#endif
