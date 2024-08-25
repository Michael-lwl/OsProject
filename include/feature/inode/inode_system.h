#ifndef INODE_SYSTEM_H
#define INODE_SYSTEM_H

#include "./../../core/data.h"
#include "./../../core/data_sizes.h"
#include "./../../core/system.h"
#include "./../../core/directory.h"
#include "./data_block.h"
#include "./indirect_blocks.h"
#include <memory>
#include <chrono>
#include <ctime>
#include <sys/types.h>
#include <vector>

class INodeSystem;

const unsigned char DIRECT_DATA_BLOCK_COUNT = 12;
const size_t INDIRECTION_BLOCK_DEPTH = 3;

class INode : public File {
    public:

        ///Copy-constructor
        INode(INode* inode): File(inode->getFilePath(), inode->getFlags(), inode->getFileSizeInBytes()),
            firstIndirectionBlock(inode->getFirstIndirectionBlock().BLOCK_SIZE),
            secondIndirectionBlock(inode->getFirstIndirectionBlock().BLOCK_SIZE),
            thirdIndirectionBlock(inode->getFirstIndirectionBlock().BLOCK_SIZE)
            {
                this->flags = inode->getFlags();
                this->numHardlinks = inode->getNumHardlinks();
                this->uid = inode->getUid();
                this->gid = inode->getGid();
                this->fileSizeInBytes = inode->getFileSizeInBytes();
                this->mtime = inode->getMtime();
                this->ctime = inode->getCtime();
                this->atime = inode->getAtime();
                // this->firstIndirectionBlock = inode->getFirstIndirectionBlock();
                // this->secondIndirectionBlock = inode->getSecondIndirectionBlock();
                // this->thirdIndirectionBlock = inode->getThirdIndirectionBlock();
                DataBlock** iNodeDataBlocks = inode->getDatablocks();
                for (size_t i = 0; i < DIRECT_DATA_BLOCK_COUNT; i++) {
                    datablocks[i] = iNodeDataBlocks[i];
                }
        }

        INode(std::string* filePath, unsigned char flags, size_t fileSizeInBytes, size_t blockSize): File(filePath, flags, fileSizeInBytes),
            firstIndirectionBlock(blockSize), secondIndirectionBlock(blockSize), thirdIndirectionBlock(blockSize)
            {
                this->flags = flags;
                numHardlinks = 0;
                uid = 0;
                gid = 0;
                this->fileSizeInBytes = fileSizeInBytes;
                time_t now_t_t = getCurrentTime();
                mtime = now_t_t;
                ctime = now_t_t;
                atime = now_t_t;
        }

        ~INode() {

        }

        bool setData(Array* data) override;
        ///Returns the actual data, without the file's system's implementation of data saving
        std::unique_ptr<Array> getData() override;

        bool appendDataBlock(std::shared_ptr<DataBlock> db);


        //Getter and Setter
        ///Returns this files' flags'
        int getFlags() {return flags;}
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
        FirstIndirectBlock getFirstIndirectionBlock() const {return firstIndirectionBlock;}
        SecondIndirectBlock getSecondIndirectionBlock() const {return secondIndirectionBlock;}
        ThirdIndirectBlock getThirdIndirectionBlock() const {return thirdIndirectionBlock;}

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
        DataBlock* datablocks[DIRECT_DATA_BLOCK_COUNT];
        FirstIndirectBlock firstIndirectionBlock;
        SecondIndirectBlock secondIndirectionBlock;
        ThirdIndirectBlock thirdIndirectionBlock;

        INodeSystem * system;
};

struct DirectoryEntry {
    size_t inode_id;
    std::string name;
};

struct INodeDirectory : public INode , public Directory {

    public:

        INodeDirectory(std::string* filePath, unsigned char flags, size_t fileSizeInBytes, size_t blockSize) :
            // File(filePath, flags, fileSizeInBytes),
            INode(filePath, (flags | Flags::IS_DIR), fileSizeInBytes, blockSize),
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

inline size_t mapToInodeSize(size_t driveSizeInByte) {
  if (driveSizeInByte <= ByteSizes::GB) {
    return INodeSizes::LE_1GB;
  }
  if (driveSizeInByte < 2 * ByteSizes::GB) {
    return INodeSizes::LT_2GB;
  }
  if (driveSizeInByte < 3 * ByteSizes::GB) {
    return INodeSizes::LT_3GB;
  }
  if (driveSizeInByte < ByteSizes::TB) {
    return INodeSizes::LE_1TB;
  }
  return INodeSizes::GT_1TB;
}

class INodeSystem : public System {
    public:

        INodeSystem(unsigned char* startPtr, Data *dataHandler, size_t driveSizeInBytes, size_t blockSize) :
            System(dataHandler, driveSizeInBytes, blockSize) {
        this->iNodeSize = mapToInodeSize(driveSizeInBytes);
        this->iNodeCount = driveSizeInBytes/iNodeSize;
        this->blockSize = blockSize;
        for (size_t i = 0; i < iNodeCount; i++) {
            //TODO: Verify that this works
            INode tmp = new (iNodes + i) INode(nullptr, 0, 0, blockSize);
            iNodes[i] = std::make_shared<INode>(tmp);
        }
        ///TODO: actually implement this!
        // auto datablockStartPtr = startPtr + sizeof(INodeSystem) + (sizeof(INode) * iNodeCount);

        // dataBlockCount = ;
        }

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
        /// Returns the file with the INodes' id.
        /// Might return null!
        std::shared_ptr<File> getFile(unsigned long iNodeId);
        float getFragmentation() override;
        bool defragDisk() override;

        std::shared_ptr<DataBlock> getNewDataBlock();

        // Getter and setter
        /// Returns this Systems NULL_PTR
        u_int64_t *NULL_PTR();

    private:
        /// Returns this Systems rootDirectory
        std::shared_ptr<File> getRoot();

        std::shared_ptr<File> getChild(std::shared_ptr<Directory> directory,
                                         const std::string &fileName);

        size_t iNodeSize;
        size_t iNodeCount;
        size_t blockSize;
        size_t driveSizeInBytes;
        size_t dataBlockCount;

        std::shared_ptr<INode>* iNodes;
        std::shared_ptr<DataBlock>* dataBlocks;
};

#endif
