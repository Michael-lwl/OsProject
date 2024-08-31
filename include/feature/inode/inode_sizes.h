#ifndef INODE_SIZES_H
#define INODE_SIZES_H

#include "../../core/data_sizes.h"

/// The following values are the defined INode-sizes in bytes.
/// The definition was taken from the following Website:
/// https://docs.oracle.com/cd/E19683-01/817-2874/fsfilesysappx-14/index.html
enum INodeSizes {
  LE_1GB = 2048,
  LT_2GB = 4096,
  LT_3GB = 6144,
  LE_1TB = 8192,
  GT_1TB = 1048576,
};

inline INodeSizes getBytesPerInode(unsigned long long driveSize) {
  if (driveSize <= getSizeInByte(ByteSizes::Gibibyte))
    return LE_1GB;
  if (driveSize <= 2 * getSizeInByte(ByteSizes::Gibibyte))
    return LT_2GB;
  if (driveSize <= 3 * getSizeInByte(ByteSizes::Gibibyte))
    return LT_3GB;
  if (driveSize <= getSizeInByte(ByteSizes::TB))
    return LE_1TB;
  if (driveSize <= getSizeInByte(ByteSizes::TB))
    return GT_1TB;
  return LT_2GB;
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
