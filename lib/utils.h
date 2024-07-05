#ifndef utils_H
#define utils_H

#include <string>

enum Color {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BLACK,
};

void safeFree(void *ptr);
///
/// creates a random String from 1 to (arbitrary) 64 bytes  length (including terminator byte)
///
unsigned char *createRandomString(unsigned int size);
unsigned char *createRandomSizedString();
unsigned int getLastNBits(unsigned int num, unsigned int n);
unsigned int getFirstNBits(unsigned int num, unsigned int n);

const std::string  colorCharCode(Color c);
std::string colorize(std::string str, Color c);
#endif
