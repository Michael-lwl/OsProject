#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

typedef std::string str;

class Command {
public:
    static const Command EXIT;
    static const Command DISK_CREATE;
    static const Command DISK_DELETE;
    static const Command DISK_CHANGE;
    static const Command DISK_WIPE;
    static const Command PARTITION_CREATE;
    static const Command PARTITION_DELETE;
    static const Command PARTITION_CHANGE;
    static const Command PARTITION_FORMAT;
    static const Command PARTITION_GET_FRAGMENTATION;
    static const Command PARTITION_DEFRAGMENT;
    static const Command FILE_CREATE;
    static const Command FILE_LIST;
    static const Command FILE_DELETE;
    static const Command FILE_INSERT;
    static const Command FILE_READ;

    //Getter
    static const std::vector<Command> getAllCommands() {
        std::vector<Command> cmds;
        cmds.reserve(17);
        cmds.push_back(Command::EXIT);
        cmds.push_back(Command::DISK_CREATE);
        cmds.push_back(Command::DISK_DELETE);
        cmds.push_back(Command::DISK_CHANGE);
        cmds.push_back(Command::DISK_WIPE);
        cmds.push_back(Command::PARTITION_CREATE);
        cmds.push_back(Command::PARTITION_DELETE);
        cmds.push_back(Command::PARTITION_CHANGE);
        cmds.push_back(Command::PARTITION_FORMAT);
        cmds.push_back(Command::PARTITION_GET_FRAGMENTATION);
        cmds.push_back(Command::PARTITION_DEFRAGMENT);
        cmds.push_back(Command::FILE_CREATE);
        cmds.push_back(Command::FILE_LIST);
        cmds.push_back(Command::FILE_DELETE);
        cmds.push_back(Command::FILE_INSERT);
        cmds.push_back(Command::FILE_READ);
        return cmds;
    }

    str getCmd() const {return this->cmd;}
    str getName() const {return this->name;}
    str getDesc() const {return this->desc;}

    Command(Command* c):
        cmd(c->cmd),
        name(c->name),
        desc(c->desc) {
    }

private:
    str cmd;
    str name;
    str desc;

    Command(str cmd, str name, str desc):
        cmd(cmd),
        name(name),
        desc(desc){
    }
};

#endif//COMMAND_H
