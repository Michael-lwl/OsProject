#ifndef STATES_H
#define STATES_H

#include "./../utils.h"

enum Status {
  RESERVED,
  CORRUPTED,
  FREE,
  USED
};

enum StatusChar {
  RESERVED_CHAR = 'R',
  CORRUPTED_CHAR = 'C',
  FREE_CHAR = '_',
  USED_CHAR = 'U',
  DEF_CHAR = 'D'
};

inline char getCharForStatus(char status) {
  switch (status) {
    case Status::RESERVED:
      return StatusChar::RESERVED_CHAR;
    case Status::CORRUPTED:
      return StatusChar::CORRUPTED_CHAR;
    case Status::FREE:
      return StatusChar::FREE_CHAR;
    case Status::USED:
      return StatusChar::USED_CHAR;
    default:
      return StatusChar::DEF_CHAR;
  }
}

inline Color getColorForStatus(char status) {
  switch (status) {
    case Status::RESERVED:
      return Color::BLUE;
    case Status::CORRUPTED:
      return Color::RED;
    case Status::FREE:
      return Color::WHITE;
    case Status::USED:
      return Color::CYAN;
    default:
      return Color::YELLOW;
  }
}

enum Flags {
  SYSTEM = 1,
  ASCII = 2,
  IS_TEMP = 4,
  IS_DIR = 8,
};

#endif
