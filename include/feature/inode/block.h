#ifndef BLOCK_H
#define BLOCK_H

#include "./../../array.h"
#include "inode_sizes.h"

class Block {

    public:
        const static unsigned int DEFAULT_SIZE = INodeSizes::LE_1TB;
        const static unsigned int MINIMUM_SIZE = INodeSizes::LE_1GB;

        ///Initializes the size. migth use the DEFAULT_SIZE, if the size is smaller than the MINIMUM_SIZE
        Block(size_t size) {
            inizializeSize(size);
        }

        static bool inizializeSize(unsigned int size) {
            if (wasInitialized) {
                return false;
            }
            blockSize = size;
            wasInitialized = true;
            return true;
        }

        virtual bool setData(Array* data)= 0;

        //Getter and Setter

        static size_t getBlockSize() {
            return (wasInitialized? blockSize : 0);
        }

    private:
        static unsigned int blockSize;
        static bool wasInitialized;
};


#endif
