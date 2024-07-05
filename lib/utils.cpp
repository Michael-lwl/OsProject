#include <stdlib.h>
#include "./utils.h"

void safeFree(void *ptr)
{
    if (ptr != nullptr)
        free(ptr);
}

///
/// creates a random String from 1 to (arbitrary) 64 bytes  length (including terminator byte)
///
unsigned char *createRandomString(unsigned int size)
{
    unsigned int n = size % 63;
    unsigned char *string = (unsigned char *)malloc((n * sizeof(char)) + 1);
    if (string == nullptr)
        return nullptr;
    if (n == 0)
        return string;
    for (unsigned int i = 0; i < n; i++)
    {
        char c = (rand() % 96) + 32;
        string[i] = c;
    }
    return string;
}

unsigned char *createRandomSizedString()
{
    return createRandomString(rand());
}

unsigned int getLastNBits(unsigned int num, unsigned int n)
{
    return num & ((1 << n) - 1);
}
unsigned int getFirstNBits(unsigned int num, unsigned int n)
{
    return num >> (sizeof(num) * 8 - n);
}

const std::string colorCharCode(Color c){
    switch (c){
        case RED:
            return std::string("\x1B[31m");
        case GREEN:
            return std::string("\x1B[32m");
        case YELLOW:
            return std::string("\x1B[33m");
        case BLUE:
            return std::string("\x1B[34m");
        case MAGENTA:
            return std::string("\x1B[35m");
        case CYAN:
            return std::string("\x1B[36m");
        case WHITE:
            return std::string("\x1B[37m");
        case BLACK:
            return std::string("\033[0m");
        default :
            return std::string("\033[0m");
    }
}

std::string colorize(std::string str, Color c) {
    return std::string(colorCharCode(c)).append(str).append(colorCharCode(WHITE));
}
