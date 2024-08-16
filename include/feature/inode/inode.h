#ifndef INODE_H
#define INODE_H

#include "./data_block.h"
#include "./indirect_blocks.h"
#include "./../../core/directory.h"
#include <ctime>
#include <sys/types.h>

const unsigned char DIRECT_DATA_BLOCK_COUNT = 12;

class INode : public File {
    public:

        INode(std::string* filePath, unsigned char flags, size_t blockSize): File(filePath, flags, 0) {
            for (DataBlock db : datablocks) {
                // db(blockSize);
            }
            firstIndirectionBlock(blockSize);
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
        int getFlags() const {return flags;}
        ///Returns the number of hardlinks
        int getNumHardlinks() const {return numHardlinks;}
        ///Returns the UserID of the creator of this file
        long getUid() const {return uid;}
        ///Returns the GroupID of the creator of this file
        long getGid() const {return gid;}
        ///Returns the Size of this file (excluding the size of the INode itself!)
        long getSizeInBytes() const {return fileSizeInBytes;}
        std::time_t getMtime() const {return mtime;}
        std::time_t getCtime() const {return ctime;}
        std::time_t getAtime() const {return atime;}
        DataBlock* getDatablocks() {return datablocks;}
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
        DataBlock datablocks[DIRECT_DATA_BLOCK_COUNT];
        FirstIndirectBlock firstIndirectionBlock;
        SecondIndirectBlock secondIndirectionBlock;
        ThirdIndirectBlock thirdIndirectionBlock;

};

struct DirectoryEntry {
    std::string name;
    size_t inode_id;
};

struct INodeDirectory : public INode , public Directory {


    bool addChild(std::shared_ptr<File> file);
    bool removeChild(std::shared_ptr<File> file);
};

#endif
