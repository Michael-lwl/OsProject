#ifndef BS_FILE_SYSTEM
#define BS_FILE_SYSTEM

#include "./../../core/system.h"
#include "./../../utils.h"
#include <cstddef>
#include <exception>
#include <memory>

#define MAX_FILE_COUNT (static_cast<unsigned int>(-1))

class BsFile;
class BsFat;

struct BsCluster
{
    ///The next cluster in the associated file, might be null if this cluster is unused or the last cluster in a File
    std::shared_ptr<BsCluster> next;
    ///The previous cluster in the associated file, might be null if this cluster is unused or the first in a File
    std::shared_ptr<BsCluster> previous;
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
        ///Creates an empty BsFile (Reserves 0 Bytes of spacec)
        BsFile(BsFat* fileSystem, std::string* filePath, unsigned char flags): BsFile(fileSystem, filePath, flags, 0) {

        }

        ~BsFile() {
            freeFile();
            filesystem = nullptr;
            clusterCount = 0;
        }

        bool setData(Array* data) override;
        std::unique_ptr<Array> getData() override;

        std::shared_ptr<BsCluster> getFileStart() const {return fileStart;}
        BsFat* getFileSystem() const {return filesystem;}
        unsigned int getClusterCount() const {return clusterCount;}

    protected:
        bool trimToSize(unsigned long newFileSize) override;
        bool expandToSize(unsigned long newFileSize) override;

    private:

        ///Sets all used clusters to empty clusters (does not empty the data)
        void freeFile();
        void removeAllClusters(std::shared_ptr<BsCluster> startCluster);
        BsFat* filesystem;
        ///The first cluster for this file
        std::shared_ptr<BsCluster>fileStart;
        ///The count of clusters used by this file
        unsigned int clusterCount;

};

class BsFat : public System
{

    public:

        BsFat(unsigned long driveSize, unsigned long blockSize, Data* dataHandler) : System(dataHandler, driveSize) {
            unsigned int blockCount = driveSize / blockSize;
            auto memory = new unsigned char[blockCount * (sizeof(BsCluster) + blockSize)];
            //auto* memory= (unsigned char *) malloc (blockCount * (sizeof(struct BsCluster) + blockSize));
            if (memory == nullptr){
                throw ("Cannot assign Memory!");
            }
            for (size_t i = 0; i < blockCount; ++i) {
                // Assign each block with custom deleter
                blocks[i] = std::shared_ptr<BsCluster>(reinterpret_cast<BsCluster*>(memory + i * sizeof(BsCluster)), [](BsCluster* ptr) {
                    ptr->~BsCluster(); // Manually call the destructor
                    delete[] reinterpret_cast<unsigned char*>(ptr); // Delete the memory
                });
            }
            this->blockCount = blockCount;
            this->blockSize = blockSize;
            this->dataPtr = std::shared_ptr<unsigned char>(memory + this->blockCount * sizeof(struct BsCluster));

            for (unsigned int i = 0; i < blockCount; i++) {
                auto* cluster = reinterpret_cast<BsCluster*>(memory + i * sizeof(struct BsCluster));
                cluster->status = Status::FREE;
                cluster->blockIndex = i;
                cluster->previous = nullptr;
                cluster->next = nullptr;
                cluster->data = memory + i * blockSize;
            }
        }

        ~BsFat() {
            for (unsigned int i = 0; i < MAX_FILE_COUNT; i++) {
                files[i].reset();
            }
            for (unsigned int i = 0; i < blockCount; i++) {
                blocks[i].reset();

            }
        }

        //Overridden function
        bool deleteFile(std::string* filePath) override;
        unsigned long getFreeSpace() override;
        unsigned long getFileCount() override;
        unsigned long getFileSize(std::string* filePath) override;
        std::shared_ptr<File> createFile(std::string* filePath, unsigned long fileSize, unsigned char flags) override;
        bool saveInFile(std::string* filePath, std::shared_ptr<Array> data) override;
        std::shared_ptr<File> getFile(std::string* filePath) override;

        //Fat functions

        ///returns false if the file array is full
        bool hasFreeFileSpace();
        /// @brief tries to get a free (random) cluster for the amount of blocks, after that it will return a nullptr
        std::shared_ptr<BsCluster> getNewCluster();
        unsigned int getFirstFreeFileIndex();

        //getter and setter
        std::shared_ptr<unsigned char> getDataPtr() const {return dataPtr;}
        unsigned long getDataSize() const {return blockSize * blockCount;}
        unsigned int getBlockSize() const {return blockSize;}
        unsigned int getBlockCount() const {return blockCount;}


    private:
        ///May return nullptr if there is no file with that name
        std::shared_ptr<BsFile> getBsFileForPath(const std::string* path);

        std::shared_ptr<BsFile> files[MAX_FILE_COUNT];
        /// The size of a block in bytes
        unsigned int blockSize;
        unsigned int blockCount;
        /// All blocks as one big array.
        ///WARNING: Its size is not "blocksize * sizeof(BsCluster)" but "blockCount * (sizeof(struct BsCluster) + blockSize)" because the data is appended (although similiarly sorted) after the Clusters
        std::shared_ptr<BsCluster>* blocks;
        ///The data (which is canonically right after the last Cluster) with the length of blockCount * blockSize
        ///Pls dont tamper with this data directly as it has to be at the same index as its paired cluster
        std::shared_ptr<unsigned char> dataPtr;
};
#endif
