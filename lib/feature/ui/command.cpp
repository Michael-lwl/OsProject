#include "./../../../include/feature/ui/command.h"

const Command Command::EXIT                         = Command("exit", "Exit Application", "Exits this Simulation.", "exit");
const Command Command::DISK_CREATE                  = Command("createDisk", "Create Disk", "Creates a disk (MBR) with the specified Byte size (Has to be less than 8 GiB)","createDisk ### byte");
const Command Command::DISK_DELETE                  = Command("deleteDisk", "Delete Disk", "Deletes the disk at the specified index.","deleteDisk #");
const Command Command::DISK_CHANGE                  = Command("changeDisk","Change Disk","Rotate (if possible) through the different drives (MBRs)","changeDisk");
const Command Command::DISK_WIPE                    = Command("wipeDisk", "Wipe Disk", "Wipes all the \"data\" at the specified disk (aka deletes all partitions in that disk).","wipeDisk #");
const Command Command::PARTITION_CREATE             = Command("createPart", "Create Partition", "Creates a partition in this MBR with the specified byte-size","createPart #### <byte> <scheme> <blocksize>");
const Command Command::PARTITION_DELETE             = Command("deletePart", "Delete Partition", "Deletes the partition in this MBR with this index","deletePart #");
const Command Command::PARTITION_CHANGE             = Command("changePart","Change Partition","Rotate (if possible) through the different partitions in this disk (MBR)","changePart");
const Command Command::PARTITION_FORMAT             = Command("formatPart", "Format Partition", "(Re-)formats the partition at this index.","formatPart # <scheme> <blocksize>");
const Command Command::PARTITION_GET_FRAGMENTATION  = Command("getFragmentation", "Get Fragmentation", "Returns the fragmentation of the specified partition","getFragmentation #");
const Command Command::PARTITION_DEFRAGMENT         = Command("defragPart","Defragment Part.","Defragments the partition if possible","defragPart #");
const Command Command::FILE_CREATE                  = Command("createFile", "Create new File", "Create a file with the specified name, size and flags. For flags please look into the documentation.","createFile <name> #### <flags>");
const Command Command::FILE_LIST                    = Command("listFiles", "List Files", "Lists all files and their flags in your current partition.","listFiles");
const Command Command::FILE_DELETE                  = Command("deleteFile", "Delete File", "Delete the file with the specified name.","deleteFile <name>");
const Command Command::FILE_INSERT                  = Command("insertFile", "Insert into file", "Sets the content of the file to the specified Data!","insertFile <name> <Real File>");
const Command Command::FILE_READ                    = Command("readFile", "Read File", "Read from the file with the specified name and writes it into the given file.","readFile <name> <Real FIle>");
