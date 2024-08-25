#ifndef INDIRECT_BLOCKS_H
#define INDIRECT_BLOCKS_H

#include "./block.h"
#include "./data_block.h"
#include <ctime>
#include <memory>
#include <sys/types.h>

class IndirectBlock: public Block {
    public:
        IndirectBlock(size_t blockSize, unsigned int maximumCapacity) : Block(blockSize) {
            this->maximumCapacity = maximumCapacity;
            this->currentCapacity = 0;
        }

        virtual bool setData(Array* data) override = 0;
        virtual Array getData() override = 0;

        //Getter and Setter
        ///Returns the maximum capacity of this IndirectBlock
        unsigned int getMaximumCapacity() const {return maximumCapacity;}

        virtual bool appendDataBlock(std::shared_ptr<DataBlock> db) = 0 ;

    protected:
        ///Returns the current capacity of this IndirectBlock
        unsigned int getCurrentCapacity() const {return currentCapacity;}
        ///Sets the current capacity of this IndirectBlock to its value.
        /// Will mostly be used in setData()
        void setCurrentCapacity(unsigned int currentCapacity) {this->currentCapacity = currentCapacity;}

    private:
        unsigned int maximumCapacity;
        unsigned int currentCapacity;
};

class FirstIndirectBlock : public IndirectBlock {
    public:
        FirstIndirectBlock(size_t blockSize) : IndirectBlock(blockSize, blockSize * blockSize) {
            // Placement new to construct DataBlock objects in the provided buffer
            for (size_t i = 0; i < blockSize; i++) {
                new (blocks + i) DataBlock(blockSize);
            }
        }

        ~FirstIndirectBlock() {
            // Explicitly call the destructor for each DataBlock
            for (size_t i = 0; i < BLOCK_SIZE; i++) {
                if (blocks[i] != nullptr) {
                    blocks[i]->~DataBlock();
                }
            }
        }

        bool setData(Array* data) override;
        Array getData() override;
        bool appendDataBlock(std::shared_ptr<DataBlock> db) override;

    private:
        DataBlock** blocks;
};

class SecondIndirectBlock : public IndirectBlock {
    public:

    SecondIndirectBlock(size_t blockSize) : IndirectBlock(blockSize, blockSize * blockSize * blockSize) {
            // Placement new to construct DataBlock objects in the provided buffer
            for (size_t i = 0; i < blockSize; i++) {
                new (blocks + (i * blockSize)) FirstIndirectBlock(blockSize);
            }
        }

        ~SecondIndirectBlock() {
            // Explicitly call the destructor for each DataBlock
            const size_t blockSize = BLOCK_SIZE;
            for (size_t i = 0; i < blockSize; i++) {
                (blocks + (i * blockSize))->~FirstIndirectBlock();
            }
        }

        bool setData(Array* data) override;
        Array getData() override;
        bool appendDataBlock(std::shared_ptr<DataBlock> db) override;

    private:
        FirstIndirectBlock* blocks;
};

class ThirdIndirectBlock : public IndirectBlock {
    public:

        ThirdIndirectBlock(size_t blockSize) : IndirectBlock(blockSize, blockSize * blockSize * blockSize * blockSize) {
            // Placement new to construct DataBlock objects in the provided buffer
            const size_t blockSizeSq = blockSize * blockSize;
            for (size_t i = 0; i < blockSize; i++) {
                new (blocks + (i * blockSizeSq)) SecondIndirectBlock(blockSize);
            }
        }

        ~ThirdIndirectBlock() {
            // Explicitly call the destructor for each DataBlock
            const size_t blockSize = BLOCK_SIZE;
            const size_t blockSizeSq = blockSize * blockSize;
            for (size_t i = 0; i < blockSize; i++) {
                (blocks + (i * blockSizeSq))->~SecondIndirectBlock();
            }
        }

        bool setData(Array* data) override;
        Array getData() override;
        bool appendDataBlock(std::shared_ptr<DataBlock> db) override;

    private:
        SecondIndirectBlock* blocks;
};

#endif
