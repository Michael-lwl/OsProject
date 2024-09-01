#include <stdlib.h>
#include <string>
#include <vector>
#include "./../include/utils.h"

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
    unsigned char *string = (unsigned char *) malloc((n * sizeof(char)) + 1);
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

std::string colorize_headless(std::string str, Color c) {
    return std::string(colorCharCode(c)).append(str);
}

std::string colorize(std::string str, Color c) {
    return colorize_headless(str, c).append(colorCharCode(WHITE));
}

std::vector<std::string> splitAt(const std::string* string, const unsigned char splitter) {
    std::vector<std::string> fi;
    size_t start = 0;
    size_t end = 0;
    while ((end = string->find(splitter, start)) != std::string::npos) {
        if (end != start) {
            fi.push_back(string->substr(start, end - start));
        }
        start = end + 1;
    }
    // Add the last segment after the last splitter
    if (start < string->length()) {
        fi.push_back(string->substr(start));
    }
    return fi;
}
