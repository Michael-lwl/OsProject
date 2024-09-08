#ifndef FILE_H
#define FILE_H

#include "../array.h"
#include <memory>
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
            (void) reservedSpaceInBytes;
        }

        virtual ~File() = default;

        ///Saves the data to the file, tries to reserve more space if needed, might resize to less space
        ///Might manipulate the data depending on the implementation of the files' system
        virtual bool setData(Array* data) = 0;
        ///Returns the actual data, without the file's system's implementation of data saving
        virtual std::unique_ptr<Array> getData() = 0;
        ///Tries to resize this file to the given size
        bool resizeFile(unsigned long newFileSize) {
            if (newFileSize == getFileSizeInBytes())
                return true;
            if (newFileSize < getFileSizeInBytes())
                return trimToSize(newFileSize);
            return expandToSize(newFileSize);
        }

        //Getter and setter

        ///Returns the reserved space for this file
        virtual unsigned long getFileSizeInBytes() = 0;
        ///Sets this files' flags
        void setFlags(unsigned char newFlags) {flags = newFlags;}
        ///Returns this files' flags
        unsigned char getFlags() const {return flags;}
        ///Returns this files' path
        std::string* getFilePath() const {return filePath;}
        ///Renames this file
        void rename(std::string* newFilePath){
            filePath = newFilePath;
        }

    protected:
        ///Resizes this file if newFileSize < this->getFileSizeInBytes(). It should call setFileSizeInBytes()!
        virtual bool trimToSize(unsigned long newFileSize) = 0;
        ///Resizes this file if this->getFileSizeInBytes() < newFileSize. It should call setFileSizeInBytes()!
        virtual bool expandToSize(unsigned long newFileSize) = 0;

        //Special getter and setter

        ///Setter for the the file size, should be used as the last call in functions like trimToSize or expandToSize
        virtual void setFileSizeInBytes(unsigned long newFileSizeInBytes)  = 0;

    private:

        ///Flags like SYSTEM, ASCII, IS_TEMP, IS_DIR
        unsigned char flags;
        std::string* filePath;

};

#endif
