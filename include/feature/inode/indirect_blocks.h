#ifndef INDIRECT_BLOCKS_H
#define INDIRECT_BLOCKS_H

#include "block.h"
#include "data_block.h"
#include <ctime>
#include <sys/types.h>

class IndirectBlock: public Block {
    public:
        IndirectBlock(size_t blockSize, unsigned int maximumCapacity) : Block(blockSize) {
            this->maximumCapacity = maximumCapacity;
            this->currentCapacity = 0;
        }

        bool setData(Array* data) override;

    protected:
        //Getter and Setter
        ///Returns the maximum capacity of this IndirectBlock
        unsigned int getMaximumCapacity() const {return maximumCapacity;}
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
            for (size_t i = 0; i < getBlockSize(); i++) {
                if (blocks[i] != nullptr) {
                    blocks[i]->~DataBlock();
                }
            }
        }

        bool setData(Array* data) override;
        unsigned int getCapacity() const {return capacity;}

    private:
        DataBlock** blocks;
        unsigned int capacity;
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
            const size_t blockSize = getBlockSize();
            for (size_t i = 0; i < blockSize; i++) {
                (blocks + (i * blockSize))->~FirstIndirectBlock();
            }
        }

        bool setData(Array* data) override;

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
            const size_t blockSize = getBlockSize();
            const size_t blockSizeSq = blockSize * blockSize;
            for (size_t i = 0; i < blockSize; i++) {
                (blocks + (i * blockSizeSq))->~SecondIndirectBlock();
            }
        }

        bool setData(Array* data) override;

    private:
        SecondIndirectBlock* blocks;
};

#endif
