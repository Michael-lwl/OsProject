#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include "./block.h"
class DataBlock : public Block {

    public:

        DataBlock(size_t size) : Block(size) {
            data = new (static_cast<void*>(this + sizeof(DataBlock))) unsigned char[size];
        }

        ~DataBlock() = default;

        ///Sets the data into this dataBlock.
        ///Returns false if the length of the data is longer than this Blocks' capacity'
        bool setData(Array* data) override;
        ///Returns the data that is contained in this DataBlock
        Array getData() override;

        unsigned char status;
        unsigned char* data;


};

#endif
