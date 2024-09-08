#include <algorithm>
#include <stdlib.h>
#include <string>
#include <vector>
#include "./../include/utils.h"

std::ostringstream* Output::os = nullptr;

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

inline const std::string colorCharCode(Color c){
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
    return colorize_headless(str, c).append(colorCharCode(BLACK));
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
std::string convertToHtmlWithColors(const std::string& input) {
    std::ostringstream output;
    bool inEscapeSequence = false;
    std::string currentCode;

    //Manual function, since regex didnt work
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == '\x1B' && input[i + 1] == '[') {
            // Start of an ANSI escape sequence
            inEscapeSequence = true;
            currentCode.clear();
            i++;  // Skip the '[' character
        } else if (inEscapeSequence) {
            if (input[i] == 'm') {
                // End of the ANSI escape sequence
                inEscapeSequence = false;

                // Map the escape code to HTML
                if (currentCode == "31") {
                    output << "<span style='color:red;'>";
                } else if (currentCode == "32") {
                    output << "<span style='color:green;'>";
                } else if (currentCode == "33") {
                    output << "<span style='color:yellow;'>";
                } else if (currentCode == "34") {
                    output << "<span style='color:blue;'>";
                } else if (currentCode == "35") {
                    output << "<span style='color:magenta;'>";
                } else if (currentCode == "36") {
                    output << "<span style='color:cyan;'>";
                } else if (currentCode == "37") {
                    output << "<span style='color:white;'>";
                } else if (currentCode == "0") {
                    output << "</span>";  // Reset (close any open span)
                }
            } else {
                // Accumulate the escape code
                currentCode += input[i];
            }
        } else {
            if (input[i] == '\n') {
                // Replace '\n' with HTML line break
                output << "<br>";
            } else if (input[i] == ' ') {
                // Replace spaces with non-breaking spaces to preserve them
                output << "&nbsp;";
            } else {
                // Regular character, just add it to the output
                output << input[i];
            }
        }
    }

    return output.str();
}
