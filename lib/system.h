#include "./data.h"
#include "./states.h"
#include "./file.h"
#include <string>

class System {

    public:
        System(Data* dataHandler){
            data = dataHandler;
        }
        virtual ~System() {
            delete data;
        }

        virtual bool deleteFile(std::string* filePath) = 0;
        virtual unsigned long getFreeSpace() = 0;
        virtual unsigned long getFileCount() = 0;
        virtual unsigned long getFileSize(std::string* filePath) = 0;
        virtual File* createFile(std::string* filePath, unsigned long fileSize, unsigned char flags) = 0;
        virtual unsigned long calcTotalSize(Array* data) = 0;

        //Getter and setter
        Data* getData() {return data;}
        void setData(Data* data) {this->data = data;}

    private:
        Data* data;

};
