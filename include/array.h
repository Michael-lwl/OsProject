#ifndef ARRAY_H
#define ARRAY_H

#include "./utils.h"
#include <cstddef>

/// Dedicated to explain, how a raw pointer was allocated
enum MemAllocation {
  MANUAL,
  AUTO,
  DONT_DELETE,
};
/// Wrapper for an array, containing the length and the pointer to the first
/// byte of an array Might be completely useless, but at least it works
class Array {
    public:
    static Array EMPTY_ARRAY;

        /// Copy-Constructor
        Array(Array *array) {
            len = array->getLength();
            arr = array->getArray();
            isMAlloc = array->isMAlloc;
        }
        // Creates an array via new unsigned char[length)]
        Array(unsigned int length) {
            len = length;
            arr = new unsigned char[length];
            isMAlloc = AUTO;
        }

        /// Creates the array, memAllocation is a flag that indicates how this array
        /// will be free'd
        Array(unsigned int length, unsigned char *array, MemAllocation memAllocation) {
            len = length;
            arr = array;
            isMAlloc = memAllocation;
        }
        ~Array() {
            switch (isMAlloc) {
                case MANUAL:
                safeFree(arr);
                break;
                case AUTO:
                delete [] arr;
                break;
                case DONT_DELETE:
                default:
                break;
            }
        }

        unsigned int getLength() const { return len; }

        unsigned char *getArray() const { return arr; }

    private:
        /// The length of the array
        unsigned int len;
        /// The single elements
        unsigned char *arr;
        /// The way this array was allocated
        MemAllocation isMAlloc;
};
#endif
