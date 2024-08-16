#ifndef DATA_SIZE_H
#define DATA_SIZE_H
#include <cstddef>

enum ByteSizes {
  /// One Byte
  Byte = 1,
  /// One Megabyte in Byte
  Megabyte = 1000,
  /// One Megabyte in Byte
  MB = 1000,
  /// One Gigabyte in Byte
  Gigabyte = 1000 * Megabyte,
  /// One Gigabyte in Byte
  GB = 1000 * MB,
  /// One Terrabyte in Byte
  Terrabyte = 1000 * Gigabyte,
  /// One Terrabyte in Byte
  TB = 1000 * GB
};

#endif
