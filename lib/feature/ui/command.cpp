#include "./../../../include/feature/ui/command.h"

const Command Command::EXIT             = Command("exit", "Exit Application", "Exits this Simulation.");
const Command Command::DISK_CREATE      = Command("createDisk ### byte", "Create Disk", "Creates a disk (MBR) with the specified Byte size (Has to be less than 8 GiB)");
const Command Command::DISK_DELETE      = Command("deleteDisk #", "Delete Disk", "Deletes the disk at the specified index.");
const Command Command::DISK_CHANGE      = Command("changeDisk","Change Disk","Rotate (if possible) through the different drives (MBRs)");
const Command Command::DISK_WIPE        = Command("wipeDisk #", "Wipe Disk", "Wipes all the \"data\" at the specified disk (aka deletes all partitions in that disk).");
const Command Command::PARTITION_CREATE = Command("createPart #### <byte> <scheme> <blocksize>", "Create Partition", "Creates a partition in this MBR with the specified byte-size");
const Command Command::PARTITION_DELETE = Command("deletePart #", "Delete Partition", "Deletes the partition in this MBR with this index");
const Command Command::PARTITION_CHANGE = Command("changePart","Change Partition","Rotate (if possible) through the different partitions in this disk (MBR)");
const Command Command::PARTITION_FORMAT = Command("formatPart # <scheme> <blocksize>", "Format Partition", "(Re-)formats the partition at this index.");
const Command Command::FILE_CREATE      = Command("createFile <name> #### <flags>", "Create new File", "Create a file with the specified name, size and flags. For flags please look into the documentation.");
const Command Command::FILE_LIST        = Command("listFiles", "List Files", "Lists all files and their flags in your current partition.");
const Command Command::FILE_DELETE      = Command("deleteFile <name>", "Delete File", "Delete the file with the specified name.");
const Command Command::FILE_INSERT      = Command("insertFile <name> <Real File>", "Insert into file", "Sets the content of the file to the specified Data!");
const Command Command::FILE_READ        = Command("readFile <name> <Real FIle>", "Read File", "Read from the file with the specified name and writes it into the given file.");
