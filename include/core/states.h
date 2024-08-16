#ifndef STATES_H
#define STATES_H

enum Status
{
    RESERVED,
    CORRUPTED,
    FREE,
    USED
};

enum StatusChar
{
    RESERVED_CHAR = 'R',
    CORRUPTED_CHAR = 'C',
    FREE_CHAR = '_',
    USED_CHAR = 'U',
    DEF_CHAR = 'D'
};
enum Flags
{
    SYSTEM = 1,
    ASCII = 2,
    IS_TEMP = 4,
    IS_DIR = 8,
};

#endif
