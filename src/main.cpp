#include <iostream>
#include <string.h>
#include <vector>
#include <cmath>
#include "../lib/utils.h"

#define TODO std::cout << "TODO: implement the rest" << std::endl;
void simpleAndClean(void *ptr)
{
    if (ptr != nullptr)
        free(ptr);
}


int main()
{
    std::cout<<colorize("T", Color::BLUE)<<"es"<<colorize("t", Color::RED)<<std::endl;
    return 0;
}
