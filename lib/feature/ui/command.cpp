#include "./../../../include/feature/ui/command.h"

const Command Command::EXIT = Command("exit", "Exit Application", "Exits this Simulation.");
const Command Command::DISK_CREATE = Command("createDisk ### byte", "Create Disk", "Creates a disk (MBR) with the specified Byte size (Has to be less than 8 GiB)");
const Command Command::DISK_DELETE = Command("deleteDisk #", "Delete Disk", "Deletes the disk at the specified index.");
const Command Command::DISK_WIPE = Command("wipeDisk #", "Wipe Disk", "Wipes all the \"data\" at the specified disk (aka deletes all partitions in that disk).");
const Command Command::PARTITION_CREATE = Command("createPart #### <byte> <scheme>", "Create Partition", "Creates a partition in this MBR with the specified byte-size");
const Command Command::PARTITION_DELETE = Command("deletePart #", "Delete Partition", "Deletes the partition in this MBR with this index");
const Command Command::PARTITION_FORMAT = Command("formatPart # <scheme>", "Format Partition", "(Re-)formats the partition at this index.");
