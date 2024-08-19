#ifndef INODE_H
#define INODE_H

#include "./data_block.h"
#include "./indirect_blocks.h"
#include "./../../core/directory.h"
#include <chrono>
#include <ctime>
#include <sys/types.h>
#include <vector>

const unsigned char DIRECT_DATA_BLOCK_COUNT = 12;

class INode : public File {
    public:

        ///Copy-constructor
        INode(INode* inode): File(inode->getFilePath(), inode->getFlags(), inode->getFileSizeInBytes()),
            firstIndirectionBlock(inode->getFirstIndirectionBlock().getBlockSize()),
            secondIndirectionBlock(inode->getFirstIndirectionBlock().getBlockSize()),
            thirdIndirectionBlock(inode->getFirstIndirectionBlock().getBlockSize()) {
                this->flags = inode->getFlags();
                this->numHardlinks = inode->getNumHardlinks();
                this->uid = inode->getUid();
                this->gid = inode->getGid();
                this->fileSizeInBytes = inode->getfileSizeInBytes();
                this->mtime = inode->getMtime();
                this->ctime = inode->getCtime();
                this->atime = inode->getAtime();
                this->firstIndirectionBlock = inode->getFirstIndirectionBlock();
                this->secondIndirectionBlock = inode->getSecondIndirectionBlock();
                this->thirdIndirectionBlock = inode->getThirdIndirectionBlock();
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
        ///Tries to resize this file to the given size, adds/removes DataBlocks accordingly
        bool resizeFile(unsigned long newFileSize) override;


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
        unsigned long getfileSizeInBytes() {return fileSizeInBytes;}
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

};

struct DirectoryEntry {
    size_t inode_id;
    std::string name;
};

struct INodeDirectory : public INode , public Directory {

    public:

        INodeDirectory(std::string* filePath, unsigned char flags, size_t fileSizeInBytes, size_t blockSize) :
            INode(filePath, (flags | Flags::IS_DIR), fileSizeInBytes, blockSize),
            Directory(filePath, (flags | Flags::IS_DIR)) {

        }

        ///Returns all the children in this directory.
        std::vector<INode*> getChildren();

        bool addChild(std::shared_ptr<File> file) override;
        bool removeChild(std::string filename) override;
        ///Overload of the removeChild method.
        ///Uses the INodeId instead of the filename. *SHOULD* be faster
        bool removeChild(size_t childId);
};

#endif
