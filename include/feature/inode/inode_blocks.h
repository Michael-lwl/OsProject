#ifndef INDIRECT_BLOCKS_H
#define INDIRECT_BLOCKS_H

#include "./block.h"
#include "./data_block.h"
#include <ctime>
#include <iostream>
#include <memory>
#include <sys/types.h>

class INodeSystem;

class IndirectBlock {
public:
  IndirectBlock() {}

  virtual ~IndirectBlock() = default;

  virtual bool setData(Array *data) = 0;
  virtual Array getData() = 0;

  virtual bool appendDataBlock(DataBlock *db, INodeSystem* system) = 0;
  virtual bool trimToSize(size_t blockCount) = 0;

  /// Returns the current amount of datablocks used
  virtual unsigned long long getLength() = 0;
  /// Returns the max amount of Bytes that can be saved in this indirect block
  virtual unsigned long long getByteCapacity() = 0;
};

class FirstIndirectBlock : public IndirectBlock, public DataBlock {
    public:
        FirstIndirectBlock(size_t blockSize): IndirectBlock(), DataBlock(blockSize) {
            blocks = reinterpret_cast<DataBlock**>(data);
            // Initialize the blocks array to nullptr
            size_t numPointers = blockSize / sizeof(DataBlock*);
            for (size_t i = 0; i < numPointers; ++i) {
                blocks[i] = nullptr;
            }
        }

        ~FirstIndirectBlock() = default;

        bool setData(Array *data) override;
        Array getData() override;
        bool appendDataBlock(DataBlock *db, INodeSystem* system) override;
        bool trimToSize(size_t blockCount) override;

        unsigned long long getLength() override;
        unsigned long long getByteCapacity() override {
            return BLOCK_SIZE * BLOCK_SIZE;
        }

        DataBlock** blocks;
};

class SecondIndirectBlock : public IndirectBlock, public DataBlock {
    public:
        SecondIndirectBlock(size_t blockSize)
            : IndirectBlock(), DataBlock(blockSize) {
                blocks = reinterpret_cast<FirstIndirectBlock**>(data);
                // Initialize the blocks array to nullptr
                size_t numPointers = blockSize / sizeof(FirstIndirectBlock*);
                for (size_t i = 0; i < numPointers; ++i) {
                    blocks[i] = nullptr;
                }
            }

            ~SecondIndirectBlock() = default;

            bool setData(Array *data) override;
            Array getData() override;
            bool appendDataBlock(DataBlock *db, INodeSystem* system) override;
            bool trimToSize(size_t blockCount) override;
            unsigned long long getLength() override;
            unsigned long long getByteCapacity() override {
                return BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE;
            }

        FirstIndirectBlock** blocks;
};

class ThirdIndirectBlock : public IndirectBlock, public DataBlock {
    public:
        ThirdIndirectBlock(size_t blockSize)
            : IndirectBlock(), DataBlock(blockSize) {
                blocks = reinterpret_cast<SecondIndirectBlock**>(data);
                // Initialize the blocks array to nullptr
                size_t numPointers = blockSize / sizeof(SecondIndirectBlock*);
                for (size_t i = 0; i < numPointers; ++i) {
                    blocks[i] = nullptr;
                }
            }

            ~ThirdIndirectBlock() = default;

        bool setData(Array *data) override;
        Array getData() override;
        bool appendDataBlock(DataBlock *db, INodeSystem* system) override;
        bool trimToSize(size_t blockCount) override;
        unsigned long long getLength() override;
        unsigned long long getByteCapacity() override {
            return BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE;
        }

        SecondIndirectBlock** blocks;
};

#endif
