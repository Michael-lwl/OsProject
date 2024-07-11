#include "./data.h"

class Data_Impl : public Data {
    public:
        Data_Impl(unsigned int elementLength):  Data(elementLength, elementLength){
        }
        ~Data_Impl(){}
        int checkData(){return 0;}
        Array* getData(Array* encodedData){return encodedData;}
        Array* encodeData(Array* data){return data;}
};
