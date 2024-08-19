#include <iostream>
#include <string.h>
#include <vector>
#include <cmath>
#include "../include/utils.h"
#include "./../include/core/data_sizes.h"

#define TODO std::cout << "TODO: implement the rest" << std::endl;
void simpleAndClean(void *ptr)
{
    if (ptr != nullptr)
        free(ptr);
}


int colouredOutputTest()
{
    std::cout<<colorize("T", Color::BLUE)<<"es"<<colorize("t", Color::RED)<<std::endl;
    return 0;
}

const size_t STD_BLOCK_SIZE = 4 * getSizeInByte(ByteSizes::KB);

int main(int argc, char** argv) {
    std::cerr<<STD_BLOCK_SIZE<<std::endl;
    /*unsigned char* FestePlatte;
    if (hasFile) {
        FestePlatte = readFile(filename);
    } else {
        FestePlatte = static_cast<unsigned char*>(malloc(sizeof(char) * ByteSizes::GB));
    }
    MBR* mbr = new (FestePlatte) MBR();
    Data* dh1 = new Data_Impl(120);
    Data* dh2 = new Data_Impl(120);
    const unsigned char* usableDrive = FestePlatte + sizeof(MBR) + 1;
    INodeSystem* inode1 = new (usableDrive) INodeSystem()
    mbr.addSystem(festePlatte[0]);*/
    return colouredOutputTest();
}
