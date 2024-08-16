#ifndef UTILS_H
#define UTILS_H

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

/// Returns the current endianess of the system (true or '1' means, that the system is Little Endian)
inline bool is_system_little_endian()
{
    const int value { 0x01 };
    const void * address { static_cast<const void *>(&value) };
    const unsigned char * least_significant_address { static_cast<const unsigned char *>(address) };

    return (*least_significant_address == 0x01);
}
///Returns the last bits of the given number, checks for endianess of the system, to get the correct smallest bits
inline unsigned int getLastNBits(unsigned int num, unsigned int n ) {
    if (is_system_little_endian())
        return (num & ((1 << n) - 1));
    return (num >> (sizeof(num) * 8 - n));
}
///Returns the last bits of the given number, checks for endianess of the system, to get the correct biggest bits
inline unsigned int getFirstNBits(unsigned int num, unsigned int n ) {
    if (is_system_little_endian())
        return (num >> (sizeof(num) * 8 - n));
    return (num & ((1 << n) - 1));
}

void safeFree(void *ptr);
///
/// creates a random String from 1 to (arbitrary) 64 bytes  length (including terminator byte)
///
unsigned char *createRandomString(unsigned int size);
unsigned char *createRandomSizedString();

///Gives the Color String for coloring in the console
const std::string  colorCharCode(Color c);
///colors the given string in the given color
//(more like tail-less, since the Color::WHITE appended string in Utils::colorize doesnt get appended)
std::string colorize_headless(std::string str, Color c);
///colors the given string in the given color and appends the Color::WHITE afterwards
std::string colorize(std::string str, Color c);
#endif
