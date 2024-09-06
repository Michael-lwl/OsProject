#ifndef COMMAND_H
#define COMMAND_H

#include <string>

typedef std::string str;

class Command {
public:
    static const Command EXIT;
    static const Command DISK_CREATE;
    static const Command DISK_DELETE;
    static const Command DISK_WIPE;
    static const Command PARTITION_CREATE;
    static const Command PARTITION_DELETE;
    static const Command PARTITION_FORMAT;

    //Getter

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
