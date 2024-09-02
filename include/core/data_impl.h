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
        int checkData(){return 0;}
        std::unique_ptr<Array> getData(Array* encodedData){return std::make_unique<Array>(encodedData);}
        std::unique_ptr<Array> encodeData(Array* data){return std::make_unique<Array>(data);}
};

#endif
