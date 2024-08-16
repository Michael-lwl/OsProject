#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "./file.h"
#include "./states.h"
#include <memory>

class Directory: public File {
    public:

        static inline bool isDirectory(File* f) {
            return ((f->getFlags() & Flags::IS_DIR) == Flags::IS_DIR);
        }

        Directory(std::string* filePath, unsigned char flags): Directory(filePath, flags | Flags::IS_DIR, 0){

        }

        Directory(std::string* filePath, unsigned char flags, unsigned long reservedSpaceInBytes): File(filePath, flags, reservedSpaceInBytes) {
        }

        ///Does not work on a Directory!
        bool setData(Array* data) {return false;};
        ///Returns the actual data, without the file's system's implementation of data saving
        virtual std::unique_ptr<Array> getData() = 0;
        ///Does not work on a Directory!
        bool resizeFile(unsigned long newFileSize) {return false;}

        ///Tries to add the file as a child of this directory
        virtual bool addChild(std::shared_ptr<File> file) = 0;

        ///Tries to remove the file as a child of this directory
        virtual bool removeChild(std::string filename) = 0;

    protected:
        ///Does not work on Directory!
        bool trimToSize(unsigned long newFileSize) {return false;}
        ///Does not work on Directory!
        bool expandToSize(unsigned long newFileSize) {return false;}

    private:

};

#endif
