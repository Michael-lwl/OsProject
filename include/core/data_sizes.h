#ifndef DATA_SIZE_H
#define DATA_SIZE_H
#include <cstddef>

enum ByteSizes {
  /// One Byte
  Byte,
  /// One Kilobyte in Byte
  Kilobyte,
  /// One Kilobyte in Byte
  KB,
  /// One Kibibyte in Byte
  Kibibyte,
  /// One Kibibyte in Byte
  KiB,
  /// One Megabyte in Byte
  Megabyte,
  /// One Megabyte in Byte
  MB,
  /// One Mebibyte in Byte
  Mebibyte,
  /// One Mebibyte in Byte
  MiB,
  /// One Gigabyte in Byte
  Gigabyte,
  /// One Gigabyte in Byte
  GB,
  /// One Gibibyte in Byte
  Gibibyte,
  /// One Gibibyte in Byte
  GiB,
  /// One Terrabyte in Byte
  Terrabyte,
  /// One Terrabyte in Byte
  TB,
  /// One Terrabyte in Byte
  Tebibyte,
  /// One Terrabyte in Byte
  TiB,
};

inline unsigned long long getSizeInByte(ByteSizes size) {
  switch (size) {
  case Byte:
    return 1LL;
  case Kilobyte:
  case KB:
    return 1000LL;
  case Kibibyte:
  case KiB:
    return 1024LL;
  case Megabyte:
  case MB:
    return 1000 * getSizeInByte(Kilobyte);
  case Mebibyte:
  case MiB:
    return 1024 * getSizeInByte(Kibibyte);
  case Gigabyte:
  case GB:
    return 1000 * getSizeInByte(Megabyte);
  case Gibibyte:
  case GiB:
    return 1024 * getSizeInByte(Mebibyte);
  case Terrabyte:
  case TB:
    return 1000 * getSizeInByte(Gigabyte);
  case Tebibyte:
  case TiB:
    return 1024 * getSizeInByte(Gibibyte);
  default:
    return getSizeInByte(ByteSizes::KB);
  }
}

enum BlockSizes {
  B_512 = 512,    // 512 Byte Blocks
  KIB_1 = 1024,   // 1 KB blocks
  KIB_2 = 2048,   // 2 KB blocks
  KIB_4 = 4096,   // 4 KB blocks
  KIB_8 = 8192,   // 8 KB blocks
  KIB_16 = 65536, // 64 KB blocks
};

#endif
