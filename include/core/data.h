#ifndef DATA_H
#define DATA_H
#include "./../array.h"
#include <memory>

///An abstract class for encoding the data that is to be saved
///If you dont want any encoding you can use Data_Impl
class Data {
    public:
        Data(unsigned int elementLength, unsigned int dataLength){
            elemLength = elementLength;
            this->dataLength = dataLength;
        }
        virtual ~Data(){elemLength = 0; dataLength = 0;}
        virtual int checkData(Array* data) = 0;
        virtual std::unique_ptr<Array> getData(Array* encodedData) = 0;
        virtual std::unique_ptr<Array> encodeData(Array* data) = 0;
        ///The max length one element will be after encoding
        unsigned int getTotalLength() const {return elemLength;}
        ///Returns the length of pure data that is saved in an element with the length of this->getTotalLength()
        unsigned int getDataLength() const {return dataLength;}

    private:
        unsigned int elemLength;
        unsigned int dataLength;
};
#endif
