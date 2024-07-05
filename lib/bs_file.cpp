#include "./bs_system.h"
#include <iostream>
void BsFile::freeFile() {
    removeAllClusters(fileStart);
}
void BsFile::removeAllClusters(BsCluster* curCluster) {
    BsCluster *tmp;
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
    ///Read the data and let it be converted from the filesystem's Data*, then save it sequencially in this file. Might need for adjusting the filesize!
    unsigned long neededSpace = filesystem->calcTotalSize(data);


    if (this->getFileSizeInBytes() < neededSpace) {
        std::cout<<"WARNING: Trying to save "<<data->getLength()
            <<" Bytes into allocated space of "<<this->getFileSizeInBytes()<<"Bytes. Resizing the file";

        if (!expandToSize(neededSpace)){
            std::cerr<<"Error: Cannot expand file \""<<this->getFilePath()<<"\" to size "<<data->getLength();
            return false;
        }
        std::cout<<"File expanded accordingly!";
    }

    BsCluster* curCluster = fileStart;
    unsigned long offset = 0;
    while (curCluster != nullptr) {
        Array* curData = new Array(filesystem->getBlockSize(),data->getArray() + offset, MemAllocation::DONT_DELETE);
        Array* encodedData = filesystem->getData()->encodeData(curData);
        setDataInCluster(curCluster, encodedData);
        curCluster = curCluster->next;
    }

    return true;
}

///Read the saved data and convert it through the filesystem's Data*, then return it
Array* BsFile::getData() {
    if (this->getFileSizeInBytes() == 0){
        return new Array((unsigned int) 0);
    }

    unsigned long arrLen = ((unsigned long) filesystem->getData()->getDataLength()) * clusterCount;
    Array* data = new Array(arrLen);

    BsCluster* curCluster = fileStart;
    unsigned int arrPtrOffset = 0;
    while (curCluster != nullptr) {
        Array* arrEncoded = new Array(filesystem->getBlockSize(), curCluster->data, MemAllocation::DONT_DELETE);
        Array* arr = filesystem->getData()->getData(arrEncoded);
        for (unsigned int i = 0; i < filesystem->getData()->getDataLength(); i++){
            data->getArray()[arrPtrOffset++] = arr->getArray()[i];
        }
        delete arrEncoded;
        delete arr;
        curCluster = curCluster->next;
    }

    return data;
}
bool BsFile::resizeFile(unsigned long newFileSize) {
    if (newFileSize == getFileSizeInBytes())
        return true;
    if (newFileSize < getFileSizeInBytes())
        return trimToSize(newFileSize);
    return expandToSize(newFileSize);

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
    BsCluster* cluster = fileStart;

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
    if (newFileSize == getFileSizeInBytes())
        return true;
    if (newFileSize < getFileSizeInBytes() || newFileSize > filesystem->getDataSize())
        return false;

    unsigned long plusOne = (newFileSize % filesystem->getBlockSize() != 0 ? 1 : 0);
    unsigned long newClusterCount = (newFileSize / (filesystem->getBlockSize())) + (plusOne);

    BsCluster* curCluster = fileStart;

    for (unsigned long i = 1; i < clusterCount; i++) {
        if (curCluster == nullptr) {
            std::cerr<<"Error: File \""<<getFilePath()<<"\" appears to be corrupt!";
            return false;
        }
        curCluster = curCluster->next;
    }
    BsCluster* next;
    unsigned long neededClusters = newClusterCount - clusterCount;

    for (unsigned long i = 1; i <= neededClusters; i++) {
        next = filesystem->getNewCluster();
        if (next == nullptr) {
            std::cerr<<"Couldnt allocate memory ("<<i<<"|"<<neededClusters<<")";
            return false;
        }
        curCluster->next = next;
        next->previous = curCluster;
        curCluster = next;
    }


    return true;
}
