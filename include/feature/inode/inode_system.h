#ifndef INODE_SYSTEM_H
#define INODE_SYSTEM_H

class INodeSystem;
#include "./../../core/data.h"
#include "./../../core/data_sizes.h"
#include "./../../core/system.h"
#include "./../../core/directory.h"
#include "./inode_blocks.h"
#include "./inode_sizes.h"
#include <memory>
#include <chrono>
#include <ctime>
#include <sys/types.h>
#include <vector>


const unsigned char DIRECT_DATA_BLOCK_COUNT = 12;
const size_t INDIRECTION_BLOCK_DEPTH = 3;

class INode : public File {
    public:

        ///Copy-constructor
        INode(INode* inode): File(inode->getFilePath(), inode->getFlags(), inode->getFileSizeInBytes())
            {
                this->flags = inode->getFlags();
                this->numHardlinks = inode->getNumHardlinks();
                this->uid = inode->getUid();
                this->gid = inode->getGid();
                this->fileSizeInBytes = inode->getFileSizeInBytes();
                this->mtime = inode->getMtime();
                this->ctime = inode->getCtime();
                this->atime = inode->getAtime();
                this->system = inode->system;
                DataBlock** iNodeDataBlocks = inode->getDatablocks();
                for (size_t i = 0; i < DIRECT_DATA_BLOCK_COUNT; i++) {
                    datablocks[i] = iNodeDataBlocks[i];
                }
                if (inode->firstIndirectionBlock) {
                    this->firstIndirectionBlock = inode->firstIndirectionBlock;
                }
                if (inode->secondIndirectionBlock) {
                    this->secondIndirectionBlock = inode->secondIndirectionBlock;
                }
                if (inode->thirdIndirectionBlock) {
                    this->thirdIndirectionBlock = inode->thirdIndirectionBlock;
                }
        }

        INode(std::string* filePath, unsigned char flags, size_t fileSizeInBytes, INodeSystem* system): File(filePath, flags, fileSizeInBytes) {
                this->flags = flags;
                numHardlinks = 0;
                uid = 0;
                gid = 0;
                this->fileSizeInBytes = fileSizeInBytes;
                time_t now_t_t = getCurrentTime();
                mtime = now_t_t;
                ctime = now_t_t;
                atime = now_t_t;
                this->system = system;
                for (size_t i = 0; i < DIRECT_DATA_BLOCK_COUNT; i++) {
                    datablocks[i] = nullptr;
                }
                this->firstIndirectionBlock = nullptr;
                this->secondIndirectionBlock = nullptr;
                this->thirdIndirectionBlock = nullptr;
        }

        ~INode() = default;

        bool setData(Array* data) override;
        ///Returns the actual data, without the file's system's implementation of data saving
        std::unique_ptr<Array> getData() override;

        bool appendDataBlock(DataBlock* db);


        //Getter and Setter
        ///Returns this files' flags'
        unsigned char getFlags() {return flags;}
        ///Sets the Flags of this INode
        void setFlags(unsigned int flags) {this->flags = flags;}
        ///Returns the number of hardlinks
        int getNumHardlinks() {return numHardlinks;}
        ///Sets the Number of Hardlinks of this INode
        void setNumHardlinks(unsigned int numHardlinks) {this->numHardlinks = numHardlinks;}
        ///Returns the UserID of the creator of this file
        long getUid() {return uid;}
        ///Sets the UserID of this INode
        void setUid(unsigned long uid) {this->uid = uid;}
        ///Returns the GroupID of the creator of this file
        long getGid() {return gid;}
        ///Sets the GroupID of this INode
        void setGid(unsigned long gid) {this->gid = gid;}
        ///Returns the Size of this file (excluding the size of the INode itself!)
        unsigned long getFileSizeInBytes() override {return fileSizeInBytes;}
        ///Returns the last modification time of this INode
        ///(Will be automatically modified when calling saveFile()!)
        std::time_t getMtime() {return mtime;}
        ///Sets the Modification-Time of this INode
        void setMtime(std::time_t mtime) {this->mtime = mtime;}
        ///Returns the creation time of this INode
        ///(Will be automatically modified when calling createFile()!)
        std::time_t getCtime() {return ctime;}
        ///Sets the Creation-Time of this INode
        void setCtime(std::time_t ctime) {this->ctime = ctime;}
        ///Returns the last accessed time of this INode
        ///(Will be automatically modified when calling getFile!)
        std::time_t getAtime() {return atime;}
        ///Sets the last Accessed-Time of this INode
        void setAtime(std::time_t atime) {this->atime = atime;}
        DataBlock** getDatablocks() {return datablocks;}
        void setDataBlocks(DataBlock** newDataBlocks);
        FirstIndirectBlock* getFirstIndirectionBlock() const {return firstIndirectionBlock;}
        void setFirstIndirectionBlock(FirstIndirectBlock* newFib) {this->firstIndirectionBlock = newFib;}
        SecondIndirectBlock* getSecondIndirectionBlock() const {return secondIndirectionBlock;}
        void setSecondIndirectionBlock(SecondIndirectBlock* newSib) {this->secondIndirectionBlock = newSib;}
        ThirdIndirectBlock* getThirdIndirectionBlock() const {return thirdIndirectionBlock;}
        void setThirdIndirectionBlock(ThirdIndirectBlock* newTib) {this->thirdIndirectionBlock = newTib;}

    protected:
        ///Resizes this file if newFileSize < this->getFileSizeInBytes()
        bool trimToSize(unsigned long newFileSize) override;
        ///Resizes this file if this->getFileSizeInBytes() < newFileSize
        bool expandToSize(unsigned long newFileSize) override;
        void setFileSizeInBytes(unsigned long newFileSize) override {fileSizeInBytes = newFileSize;};

    private:
        unsigned int flags;
        unsigned int numHardlinks;
        unsigned long uid;
        unsigned long gid;
        unsigned long fileSizeInBytes;
        std::time_t mtime;
        std::time_t ctime;
        std::time_t atime;
        INodeSystem * system;
        DataBlock* datablocks[DIRECT_DATA_BLOCK_COUNT];
        FirstIndirectBlock* firstIndirectionBlock;
        SecondIndirectBlock* secondIndirectionBlock;
        ThirdIndirectBlock* thirdIndirectionBlock;

};

struct DirectoryEntry {
    size_t inode_id;
    std::string name;
};

struct INodeDirectory : public INode , public Directory {

    public:

        INodeDirectory(std::string* filePath, unsigned char flags, size_t fileSizeInBytes, INodeSystem* system) :
            // File(filePath, flags, fileSizeInBytes),
            INode(filePath, (flags | Flags::IS_DIR), fileSizeInBytes, system),
            Directory(filePath, (flags | Flags::IS_DIR)) {

            }

        ///Returns all the children in this directory.
        std::vector<INode*> getChildren();

        /// The directory assumes, that the File is an inode
        bool addChild(std::shared_ptr<File> file) override;
        bool removeChild(std::string filename) override;

        bool setData(Array* data) override {
            (void) data;
            return false;
        };
        ///Returns an array of the saved DirectionEntries
        std::unique_ptr<Array> getData() override;

        ///Overload of the removeChild method.
        ///Uses the INodeId instead of the filename. *SHOULD* be faster
        bool removeChild(size_t childId);

        bool trimToSize(size_t newSize) override {
            (void) newSize;
            return false;
        };
        bool expandToSize(size_t newSize) override {
            (void) newSize;
            return false;
        };

    private:
        std::vector<DirectoryEntry> files;
};

class INodeSystem : public System {
    public:
        static size_t calculateInodeSize(size_t ptrSize) {
            return sizeof(INode) + (DIRECT_DATA_BLOCK_COUNT * ptrSize);
        }

        static long long calculateNumberOfInodes(long long driveSizeNoOverhead, int inodeSize) {
            // Step 1: Initial calculation of inodes
            INodeSizes numberOfBytesPerInode = getBytesPerInode(driveSizeNoOverhead);
            long long initialNumberOfInodes = driveSizeNoOverhead / numberOfBytesPerInode;

            // Step 2: Calculate the space used by the inodes themselves
            long long spaceUsedByInodes = initialNumberOfInodes * inodeSize;

            // Step 3: Adjust the usable space by subtracting the space used by inodes
            long long adjustedDriveSize = driveSizeNoOverhead - spaceUsedByInodes;

            // Step 4: Recalculate the number of inodes based on the adjusted space
            long long finalNumberOfInodes = adjustedDriveSize / numberOfBytesPerInode;

            return finalNumberOfInodes;
        }

        static INodeSystem* create(void* memory, unsigned long driveSize, BlockSizes blockSize, Data* dataHandler) {
            size_t inodeSystemSize = sizeof(INodeSystem);
            if (driveSize <= inodeSystemSize) {
                return nullptr;
            }
            // Remaining space for inodes and their data blocks
            unsigned long driveSizeNoOverhead = driveSize - inodeSystemSize;
            INodeSizes numberOfBytesPerInode = getBytesPerInode(driveSize);
            size_t inodeSize = calculateInodeSize(sizeof(DataBlock*));

            size_t inodeCount = calculateNumberOfInodes(driveSizeNoOverhead, inodeSize);
            // If we cannot allocate any blocks, why should we allocate at all?
            if (inodeCount == 0) {
                return nullptr;
            }
            size_t reservedSpaceForINodes = inodeCount * inodeSize;

            size_t remainingDataBlocks = driveSizeNoOverhead - reservedSpaceForINodes;

            *SysOut() << "-------------------------------------------------------"
                          << "\nTotal size of drive:      " << driveSize
                          << "\nUsable size of drive:     " << driveSizeNoOverhead
                          << "\nINodeCount:               " << inodeCount
                          << "\nINodeSize:                " << inodeSize
                          << "\nSize of Space for INodes: " << reservedSpaceForINodes
                          << "\nremainingDataBlocks size: " << remainingDataBlocks
                          << "\nBytes covered per Inodes: " << numberOfBytesPerInode
                          << "\n-------------------------------------------------------"
                          << std::endl;

            if (remainingDataBlocks < ((driveSize * 4) / 5)) {
                std::cerr << "Error: calculated size of INodes is too big, drive wont be usable!" << std::endl;
                return nullptr;
            }

            INodeSystem* inodeSystem = new (memory) INodeSystem(driveSize, blockSize, dataHandler, inodeCount);

            return inodeSystem;
        }

        ~INodeSystem() = default;

        ///Overriden Functions

        /// Deletes the file with the specified path, and only the specified file
        bool deleteFile(std::string *filePath) override;
        /// Returns the accessible free space of this partition
        unsigned long getFreeSpace() override;
        /// Returns the amount of stored files
        unsigned long getFileCount() override;
        /// Returns the size of the specified file
        unsigned long getFileSize(std::string *filePath) override;
        /// Creates a file on the specified path. It sets the flags and reserves the
        /// specified space. (If you want to save the content, you would have to save
        ///it via saveInFile afterwards) Generally strifes to be done in O(n)
        std::shared_ptr<File> createFile(std::string *filePath,
                                           unsigned long fileSize, unsigned char flags) override;
        /// Saves the content in the specified file, might resize it, if the reserved
        /// space is too small.
        bool saveInFile(std::string *filePath, std::shared_ptr<Array> data) override;
        /// Returns the file associated with the specified path.
        /// Might return null!
        std::shared_ptr<File> getFile(std::string *filePath) override;
        /// Returns the file via the INodes' id.
        /// Might return null!
        std::shared_ptr<File> getFile(unsigned long iNodeId);

        std::vector<std::shared_ptr<File>> getAllFiles() override ;
        float getFragmentation() override;
        bool defragDisk() override;

        DataBlock* getNewDataBlock(unsigned char status = Status::USED);
        FirstIndirectBlock* getNewFirstIndirectBlock();
        SecondIndirectBlock* getNewSecondIndirectBlock();
        ThirdIndirectBlock* getNewThirdIndirectBlock();

        bool boot() override { return false;}

        char getCharForObjective(DataBlock* block);

        void show() override;

        //Getter and Setter

        size_t getBlockCount() {return dataBlockCount;}

        /// Returns the INode at the specific index
        INode* getINode(size_t iNodeIndex) {
            if (iNodeIndex >= iNodeCount) {
                std::string bIStr = std::to_string(iNodeIndex);
                throw std::out_of_range(std::string("DataBlockIndex ").append(bIStr).append(" out of bounds"));
            }
            return reinterpret_cast<INode*>(reinterpret_cast<unsigned char*>(this) + sizeof(INodeSystem) + (iNodeIndex * sizeof(INode)));
        }
        // Getter for a datablock at a specific index
        DataBlock* getDataBlock(size_t blockIndex) {
            if (blockIndex >= dataBlockCount) {
                std::string bIStr = std::to_string(blockIndex);
                throw std::out_of_range(std::string("DataBlockIndex ").append(bIStr).append(" out of bounds"));
            }
            return reinterpret_cast<DataBlock*>(reinterpret_cast<unsigned char*>(this) + sizeof(INodeSystem) + (iNodeCount * sizeof(INode)) + (blockIndex * BLOCK_SIZE));
        }

        const size_t iNodeSize;
        const size_t iNodeCount;
        const size_t dataBlockCount;
    private:

      INodeSystem(size_t driveSizeInBytes, BlockSizes blockSize, Data* dataHandler, size_t iNodeCount) :
        System(dataHandler, driveSizeInBytes, blockSize) ,
        iNodeSize(getBytesPerInode(driveSizeInBytes)),
        iNodeCount(iNodeCount),
        dataBlockCount(((driveSizeInBytes - iNodeCount * sizeof(INode) - sizeof(INodeSystem)) - ((driveSizeInBytes - iNodeCount * sizeof(INode) - sizeof(INodeSystem)) % blockSize)) / blockSize) {
          this->iNodes = getINode(0);
          for (size_t i = 0; i < iNodeCount; i++) {
            new (this->iNodes + i) INode(nullptr, 0, 0, this);
          }
          for (size_t i = 0; i < dataBlockCount; i++) {
            new (getDataBlock(i)) DataBlock(BLOCK_SIZE - sizeof(DataBlock));
          }
      }

        /// Returns this Systems rootDirectory
        std::shared_ptr<File> getRoot();

        std::shared_ptr<File> getChild(Directory* directory,
                                         const std::string &fileName);
        INode* findFile(std::string* filePath);

        INode* iNodes;
};

#endif
