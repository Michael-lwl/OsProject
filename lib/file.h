#include "./array.h"
#include <string>

class File {
    public:
        ///Creates an empty file (this->getFileSizeInByte() == 0)
        File(std::string* filePath, unsigned char flags): File(filePath, flags, 0){
            //Create Empty File
        }
        File(std::string* filePath, unsigned char flags, unsigned long reservedSpaceInBytes) {
            this->filePath = filePath;
            this->flags = flags;
            fileSizeInBytes = reservedSpaceInBytes;
        }

        virtual ~File() {
            delete filePath;
            flags = 0;
            fileSizeInBytes = 0;
        }

        virtual bool setData(Array* data) = 0;
        virtual Array* getData() = 0;
        virtual bool resizeFile(unsigned long newFileSize) = 0;

        //Getter and setter
        unsigned long getFileSizeInBytes() {return fileSizeInBytes;}
        void setFlags(unsigned char newFlags){flags = newFlags;}
        unsigned char getFlags(){return flags;}
        const std::string* getFilePath() {return filePath;}
        void rename(std::string* newFilePath){
            delete filePath;
            filePath = newFilePath;
        }

    protected:
        virtual bool trimToSize(unsigned long newFileSize) = 0;
        virtual bool expandToSize(unsigned long newFileSize) = 0;

        //Special getter and setter
        void setFileSizeInBytes(unsigned long newFileSizeInBytes) {fileSizeInBytes = newFileSizeInBytes;}

    private:

        unsigned long fileSizeInBytes;
        ///Flags like SYSTEM, ASCII, IS_TEMP, IS_DIR
        unsigned char flags;
        const std::string* filePath;

};
