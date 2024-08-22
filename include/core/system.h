#include "./data.h"
#include "./states.h"
#include "./file.h"
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>

///The interface for interacting with a file-system
class System {

    public:

        const size_t BLOCK_SIZE;

        System(Data* dataHandler, size_t driveSize, size_t blockSize): BLOCK_SIZE(blockSize){
            if (dataHandler == nullptr) {
                throw std::exception();
            }
            dataHandler = dataHandler;
            this->driveSize = driveSize;
        }
        virtual ~System() {
            delete dataHandler;
        }

        ///Deletes the file with the specified path, and only the specified file
        virtual bool deleteFile(std::string* filePath) = 0;
        ///Returns the accessible free space of this partition
        virtual unsigned long getFreeSpace() = 0;
        ///Returns the amount of stored files
        virtual unsigned long getFileCount() = 0;
        ///Returns the size of the specified file
        virtual unsigned long getFileSize(std::string* filePath) = 0;
        ///Creates a file on the specified path. It sets the flags and reserves the specified space.
        ///(If you want to save the content, you would have to save it via saveInFile afterwards)
        ///Generally strifes to be at least done in O(n)
        virtual std::shared_ptr<File> createFile(std::string* filePath, unsigned long fileSize, unsigned char flags) = 0;
        ///Saves the content in the specified file, might resize it, if the reserved space is too small.
        virtual bool saveInFile(std::string* filePath, std::shared_ptr<Array> data) = 0;
        ///Returns a the file associated with the specified path.
        ///Might return null!
        virtual std::shared_ptr<File> getFile(std::string* filePath) = 0;
        ///Returns the disks fragmentation.
        ///Please return 0, if defragmentation is not necessary in the filesystem.
        virtual float getFragmentation() = 0;
        ///Defragments the disk. Returns true if it ran successfully!
        ///Please also return true, if defragmentation is not necessary in the filesystem.
        virtual bool defragDisk() = 0;
        ///Returns the space the data will occupy after "Encoding" it with this systems DataHandler
        inline unsigned long calcTotalSize(Array* data) {
            const unsigned long dataLength = getDataHandler()->getDataLength();
            const unsigned long totalLength = getDataHandler()->getTotalLength();
            const unsigned long iterations = data->getLength() / dataLength;
            return iterations * totalLength;
        };

        //Getter and setter
        ///Returns this Systems DataHandler (Primary used for encoding a file)
        Data* getDataHandler() const {return dataHandler;}
        ///Sets the DataHandler
        ///WARNING: it doesnt change the saved file encodings by default, you *NEED* to implement it yourself!
        void setDataHandler(Data* data) {this->dataHandler = data;}

        size_t getDriveSize() const {return driveSize;}

    protected:
        ///This Systems NULL_PTR
        u_int16_t* NULL_PTR;

    private:
        Data* dataHandler;
        size_t driveSize;

};
