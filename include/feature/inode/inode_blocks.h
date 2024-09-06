#ifndef INDIRECT_BLOCKS_H
#define INDIRECT_BLOCKS_H

class Block;
class DataBlock;
class FirstIndirectBlock;
class SecondIndirectBlock;
class ThirdIndirectBlock;

#include "./inode_sizes.h"
#include "./../../core/data_sizes.h"
#include "./../../core/states.h"
#include "./../../array.h"
#include <ctime>
#include <iostream>
#include <memory>
#include <sys/types.h>


#ifndef INODE_SYSTEM_H
#define INODE_SYSTEM_H

class INodeSystem {
    public:
        DataBlock* getNewDataBlock(unsigned char status = Status::RESERVED);
        FirstIndirectBlock* getNewFirstIndirectBlock();
        SecondIndirectBlock* getNewSecondIndirectBlock();
};

#endif

enum AdditionalStats {
  INDIRECT_1 = 4,
  INDIRECT_2 ,
  INDIRECT_3
};
enum AdditionalStatsChars {
  INDIRECT_1_CHAR = 'X',
  INDIRECT_2_CHAR = 'Y',
  INDIRECT_3_CHAR = 'Z'
};

class Block {
    public:
        const static unsigned int DEFAULT_SIZE = INodeSizes::LE_1TB;
        const static unsigned int MINIMUM_SIZE = INodeSizes::LE_1GB;

        const size_t BLOCK_SIZE;

        ///Initializes the size. migth use the DEFAULT_SIZE, if the size is smaller than the MINIMUM_SIZE
        Block(size_t blockSize) : BLOCK_SIZE(blockSize) {

        }
        
        virtual ~Block() = default;

        virtual bool setData(Array* data) = 0;
        virtual Array getData() = 0;
};

class DataBlock : public Block {
    public:
        DataBlock(size_t size) : Block(size) {
            data = new (static_cast<void*>(this + sizeof(DataBlock))) unsigned char[size];
            status = Status::FREE;
        }

        virtual ~DataBlock() = default;

        ///Sets the data into this dataBlock.
        ///Returns false if the length of the data is longer than this Blocks' capacity'
        bool setData(Array* data) override;
        ///Returns the data that is contained in this DataBlock
        Array getData() override;

        ///Literally padding for showing the correct values. Do not access!
        unsigned long padding = 0;
        unsigned char status;
        unsigned char* data;
};

class IndirectBlock {
    public:
        virtual ~IndirectBlock() = default;

        virtual bool setData(Array *data) = 0;
        virtual Array getData() = 0;

        virtual bool appendDataBlock(DataBlock *db, INodeSystem* system) = 0;
        virtual bool trimToSize(size_t blockCount) = 0;

        /// Returns the current amount of datablocks used
        virtual unsigned long long getLength() = 0;
        /// Returns the maximum amount of blocks that could be saved in this Block
        virtual unsigned long long getCapacity() = 0;
        /// Returns the max amount of Bytes that can be saved in this indirect block
        virtual unsigned long long getByteCapacity() = 0;
};

class FirstIndirectBlock : public IndirectBlock, public Block {
    public:
        FirstIndirectBlock(size_t blockSize): Block(blockSize) {
            data = new (static_cast<void*>(this + sizeof(FirstIndirectBlock))) DataBlock*[blockSize];
            status = AdditionalStats::INDIRECT_1;
            // Initialize the blocks array to nullptr
            size_t numPointers = blockSize / sizeof(DataBlock*);
            for (size_t i = 0; i < numPointers; ++i) {
                data[i] = nullptr;
            }
        }

        ~FirstIndirectBlock() = default;

        bool setData(Array *data) override;
        Array getData() override;
        bool appendDataBlock(DataBlock *db, INodeSystem* system) override;
        bool trimToSize(size_t blockCount) override;

        unsigned long long getLength() override;
        unsigned long long getCapacity() override {
            return CAPACITY(BLOCK_SIZE);
        }
        static inline unsigned long long CAPACITY(size_t BLOCK_SIZE) {
            return (BLOCK_SIZE- sizeof(FirstIndirectBlock))/sizeof(DataBlock*);
        }
        unsigned long long getByteCapacity() override {
            return this->getCapacity() * BLOCK_SIZE;
        }
        unsigned char status;
        DataBlock** data;
};

class SecondIndirectBlock : public IndirectBlock, public Block {
    public:
        SecondIndirectBlock(size_t blockSize) : Block(blockSize) {
                data = new (static_cast<void*>(this + sizeof(SecondIndirectBlock))) FirstIndirectBlock*[blockSize];
                status = AdditionalStats::INDIRECT_2;
                // Initialize the blocks array to nullptr
                size_t numPointers = blockSize / sizeof(FirstIndirectBlock*);
                for (size_t i = 0; i < numPointers; ++i) {
                    data[i] = nullptr;
                }
            }

            ~SecondIndirectBlock() = default;

            bool setData(Array *data) override;
            Array getData() override;
            bool appendDataBlock(DataBlock *db, INodeSystem* system) override;
            bool trimToSize(size_t blockCount) override;
            unsigned long long getLength() override;
            unsigned long long getCapacity() override {
                return CAPACITY(BLOCK_SIZE);
            }
            static inline unsigned long long CAPACITY(size_t BLOCK_SIZE) {
                return (BLOCK_SIZE- sizeof(SecondIndirectBlock))/ sizeof(FirstIndirectBlock*) * FirstIndirectBlock::CAPACITY(BLOCK_SIZE);
            }
            unsigned long long getByteCapacity() override {
                return this->getCapacity() * BLOCK_SIZE;
            }
        unsigned char status;
        FirstIndirectBlock** data;
};

class ThirdIndirectBlock : public IndirectBlock, public Block {
    public:
        ThirdIndirectBlock(size_t blockSize) : IndirectBlock(), Block(blockSize) {
            data = new (static_cast<void*>(this + sizeof(ThirdIndirectBlock))) SecondIndirectBlock*[blockSize];
            status = AdditionalStats::INDIRECT_3;
            // Initialize the blocks array to nullptr
            size_t numPointers = blockSize / sizeof(SecondIndirectBlock*);
            for (size_t i = 0; i < numPointers; ++i) {
                data[i] = nullptr;
            }
        }

        ~ThirdIndirectBlock() = default;

        bool setData(Array *data) override;
        Array getData() override;
        bool appendDataBlock(DataBlock *db, INodeSystem* system) override;
        bool trimToSize(size_t blockCount) override;
        unsigned long long getLength() override;
        unsigned long long getCapacity() override {
            return CAPACITY(BLOCK_SIZE);
        }
        static inline unsigned long long CAPACITY(size_t BLOCK_SIZE) {
            return (BLOCK_SIZE - sizeof(ThirdIndirectBlock))/ sizeof(ThirdIndirectBlock*) * SecondIndirectBlock::CAPACITY(BLOCK_SIZE);
        }
        unsigned long long getByteCapacity() override {
            return this->getCapacity() * BLOCK_SIZE;
        }
        unsigned char status;
        SecondIndirectBlock** data;
};

#endif
