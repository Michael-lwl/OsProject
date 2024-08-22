#ifndef BLOCK_H
#define BLOCK_H

#include "./../../array.h"
#include "./inode_sizes.h"

class Block {

    public:
        const static unsigned int DEFAULT_SIZE = INodeSizes::LE_1TB;
        const static unsigned int MINIMUM_SIZE = INodeSizes::LE_1GB;

        const size_t BLOCK_SIZE;

        ///Initializes the size. migth use the DEFAULT_SIZE, if the size is smaller than the MINIMUM_SIZE
        Block(size_t blockSize) : BLOCK_SIZE(blockSize) {

        }

        virtual bool setData(Array* data) = 0;
        virtual Array getData() = 0;

};


#endif
