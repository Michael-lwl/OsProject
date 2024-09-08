#ifndef DATA_IMPL_H
#define DATA_IMPL_H

#include "./data.h"
#include "./../array.h"
#include <memory>

///This class is just a mirroring Data-Encoder, so it returns the values you give it
class Data_Impl : public Data {
    public:
        Data_Impl(unsigned int elementLength):  Data(elementLength, elementLength){
        }
        ~Data_Impl(){}
        virtual int checkData(Array* data){(void) data; return 0;}
        virtual std::unique_ptr<Array> getData(Array* encodedData){return std::make_unique<Array>(encodedData);}
        virtual std::unique_ptr<Array> encodeData(Array* data){return std::make_unique<Array>(data);}
};

#endif
