#include "./../../../include/feature/fat/bs_system.h"
#include "./../../../include/array.h"
#include <iostream>
#include <memory>
void BsFile::freeFile() {
    removeAllClusters(fileStart);
}
void BsFile::removeAllClusters(std::shared_ptr<BsCluster> curCluster) {
    std::shared_ptr<BsCluster> tmp;
    while (curCluster != nullptr){
        curCluster->status = Status::FREE;
        curCluster->previous = nullptr;
        tmp = curCluster;
        curCluster = curCluster->next;
        tmp->next = nullptr;
    }
}

void setDataInCluster(BsCluster* cluster, Array* data) {
    for (unsigned long i = 0; i < data->getLength(); i++) {
        cluster->data[i] = data->getArray()[i];
    }
}

bool BsFile::setData(Array* data) {
    using namespace std;
    ///Read the data and let it be converted from the filesystem's Data*, then save it sequencially in this file. Might need for adjusting the filesize!
    unsigned long neededSpace = filesystem->calcTotalSize(data);


    if (this->getFileSizeInBytes() < neededSpace) {
        cout<<"WARNING: Trying to save "<<data->getLength()
            <<" Bytes into allocated space of "<<this->getFileSizeInBytes()<<"Bytes. Resizing the file"<<endl;

        if (!expandToSize(neededSpace)){
            cerr<<"Error: Cannot expand file \""<<this->getFilePath()<<"\" to size "<<data->getLength()<<endl;
            return false;
        }
        cout<<"File expanded accordingly!"<<endl;
    }

    shared_ptr<BsCluster> curCluster = fileStart;
    unsigned long offset = 0;
    while (curCluster != nullptr) {
        unique_ptr<Array> curData = make_unique<Array>(filesystem->getBlockSize(),data->getArray() + offset, MemAllocation::DONT_DELETE);
        unique_ptr<Array> encodedData = filesystem->getDataHandler()->encodeData(curData.get());
        setDataInCluster(curCluster.get(), encodedData.get());
        curCluster = curCluster->next;
    }

    return true;
}

///Read the saved data and convert it through the filesystem's Data*, then return it
std::unique_ptr<Array> BsFile::getData() {
    using namespace std;
    if (this->getFileSizeInBytes() == 0){
        return make_unique<Array>((unsigned int) 0);
    }

    unsigned long arrLen = ((unsigned long) filesystem->getDataHandler()->getDataLength()) * clusterCount;
    unique_ptr<Array> data = make_unique<Array>(arrLen);

    BsCluster* curCluster = fileStart.get();
    unsigned int arrPtrOffset = 0;
    while (curCluster != nullptr) {
        shared_ptr<Array> arrEncoded = make_shared<Array>(filesystem->getBlockSize(), curCluster->data, MemAllocation::DONT_DELETE);
        unique_ptr<Array> arr = filesystem->getDataHandler()->getData(arrEncoded.get());
        for (unsigned int i = 0; i < filesystem->getDataHandler()->getDataLength(); i++){
            data->getArray()[arrPtrOffset++] = arr->getArray()[i];
        }
        curCluster = curCluster->next.get();
    }

    return data;
}

bool BsFile::trimToSize(unsigned long newFileSize) {
    if (newFileSize == getFileSizeInBytes())
        return true;
    if ((newFileSize > getFileSizeInBytes()) || (newFileSize > filesystem->getDataSize()))
        return false;
    unsigned long plusOne = (newFileSize % filesystem->getBlockSize() != 0 ? 1 : 0);
    unsigned long newClusterCount = (newFileSize / (filesystem->getBlockSize())) + (plusOne);
    /*
        full size= 256mb
        clusterSize 32 mb
        [] [] [] [] [] [] [] []|
        32 32 32 32 32 32 32 32|
        size = 200mb
        -> 200/ 32 = 6
        [] [] [] [] [] []|[] []
        32 32 32 32 32 32|32 32

    */
    std::shared_ptr<BsCluster> cluster = fileStart;

    for (unsigned long i = 1; i < newClusterCount; i++) {
        if (cluster->next == nullptr){
            return false;
        }
        cluster = cluster->next;
    }
    removeAllClusters(cluster);

    return true;
}

bool BsFile::expandToSize(unsigned long newFileSize) {
    using namespace std;

    if (newFileSize == getFileSizeInBytes())
        return true;
    if (newFileSize < getFileSizeInBytes() || newFileSize > filesystem->getDataSize())
        return false;

    unsigned long plusOne = (newFileSize % filesystem->getBlockSize() != 0 ? 1 : 0);
    unsigned long newClusterCount = (newFileSize / (filesystem->getBlockSize())) + (plusOne);

    shared_ptr<BsCluster> curCluster = fileStart;

    for (unsigned long i = 1; i < clusterCount; i++) {
        if (curCluster == nullptr) {
            cerr<<"Error: File \""<<getFilePath()<<"\" appears to be corrupt!"<<endl;
            return false;
        }
        curCluster = curCluster->next;
    }
    shared_ptr<BsCluster> next;
    unsigned long neededClusters = newClusterCount - clusterCount;

    for (unsigned long i = 1; i <= neededClusters; i++) {
        next = filesystem->getNewCluster();
        if (next == nullptr) {
            std::cerr<<"Couldnt allocate memory ("<<i<<"|"<<neededClusters<<")"<<endl;
            return false;
        }
        curCluster->next = next;
        next->previous = curCluster;
        curCluster = next;
    }

    return true;
}
