#ifndef DATA_H
#define DATA_H
#include "./array.h"
#include <memory>

class Data {
    public:
        Data(unsigned int elementLength, unsigned int dataLength){
            elemLength = elementLength;
            this->dataLength = dataLength;
        }
        virtual ~Data(){elemLength = 0; dataLength = 0;}
        virtual int checkData() = 0;
        virtual std::unique_ptr<Array> getData(Array* encodedData) = 0;
        virtual std::unique_ptr<Array> encodeData(Array* data) = 0;
        unsigned int getTotalLength() {return elemLength;}
        unsigned int getDataLength() {return dataLength;}

    private:
        unsigned int elemLength;
        unsigned int dataLength;
};
#endif
