#ifndef INODE_SIZES_H
#define INODE_SIZES_H

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

/// 
enum BlockSizes {
    BLOCK_SIZE_LE_1GB = 4096,    // 4 KB blocks for LE_1GB
    BLOCK_SIZE_LT_2GB = 4096,    // 4 KB blocks for LT_2GB
    BLOCK_SIZE_LT_3GB = 8192,    // 8 KB blocks for LT_3GB
    BLOCK_SIZE_LE_1TB = 8192,    // 8 KB blocks for LE_1TB
    BLOCK_SIZE_GT_1TB = 65536,   // 64 KB blocks for GT_1TB (or 1 MB depending on the system)
};


#endif
