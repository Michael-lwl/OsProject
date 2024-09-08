#include "./../../../include/feature/ui/mainwindow.h"
#include "./../../../include/utils.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <qt/QtWidgets/qlayout.h>
#include <qt/QtWidgets/qlayoutitem.h>
#include <qt/QtWidgets/qpushbutton.h>
#include <qt/QtWidgets/qwidget.h>
#include <cmath>
#include <filesystem>
///Clears the layout recursively
void clearLayout(QLayout* layout, bool deleteWidgets = true)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

void MainWindow::setCommand(Command c) {
    this->commandInput->clear();
    this->commandInput->insert(c.getCmd().c_str());
}

void MainWindow::setHelpCommands(std::vector<Command> commands) {
    clearLayout(this->buttonLayout);
    for (Command &c : commands) {
        QPushButton* btn = new QPushButton(c.getName().c_str());
        QObject::connect(btn, &QPushButton::clicked, this, [this, c]() {
            this->setCommand(c);
        });
        btn->setToolTip(c.getDesc().c_str());
        this->buttonLayout->addWidget(btn);
    }
}

SpeicherSystem stringToSpeicherSystem(std::string input){
    if(input == "BS_Fat") return SpeicherSystem::BS_Fat;
    if(input == "INodeSystem") return SpeicherSystem::INodeSystem;
    return SpeicherSystem::BS_Fat;
}
ByteSizes stringToByteSize(std::string input){ //Todo überprüfen ob das im richtigen Progrmam überhaupt funktionieren wird
    if(input == "Byte") return ByteSizes::Byte;
    return ByteSizes::Byte;
};

BlockSizes stringToBlockSize(std::string input){
    if(input == "B_512") return BlockSizes::B_512;
    return BlockSizes::B_512;
}
Flags stringToFlags(std::string input){
    if(input == "ASCII") return Flags::ASCII;
    return ASCII;
}
bool MainWindow::handleCommand(str command) {
    //Fast exit
    if (command.compare(Command::EXIT.getCmd()) == 0) {
        QCoreApplication::quit();
        return true;
    }
    std::vector<str> userInput = splitAt(&command, ' ');
    if (userInput.empty()) return false;
    for (const Command &c : Command::getAllCommands()) {
        std::string cmd = c.getCmd();
        std::vector<str> cmdAndParams = splitAt(&cmd, ' ');
        //If not same amount of params skip
        if (cmdAndParams.size() > userInput.size()) {
            continue;
        }
        //If not same command skip
        if (userInput.at(0) != cmdAndParams.at(0)) {
            continue;
        }
    }
    if (userInput.at(0) == "createDisk" && userInput.size() == 3){
        unsigned long long size = std::stoull(userInput[1]);
        ByteSizes byteSize = ByteSizes::Byte;
        return createDisk(size, byteSize);
    }
    if (userInput.at(0) == "deleteDisk" && userInput.size() == 2){
        unsigned int index = std::stoull(userInput[1]);
        return deleteDisk(index);
    }
    if (userInput.at(0) == "changeDisk" && userInput.size() == 1){

        return changeDisk();
    }
    if (userInput.at(0) == "wipeDisk" && userInput.size() == 2){
        unsigned int index = std::stoull(userInput[1]);
        return wipeDisk(index);
    }
     if (userInput.at(0) == "createPart" && userInput.size() == 5){
        unsigned long long size = std::stoull(userInput[1]);
        ByteSizes byteSize = stringToByteSize(userInput.at(2));        //TODO herausfinden wie UserInput zu enum
        SpeicherSystem system = stringToSpeicherSystem(userInput.at(3));
        BlockSizes blockSize =  stringToBlockSize(userInput.at(4));
        return createPart(size,byteSize,system,blockSize);
    }

    if (userInput.at(0) == "deletePart" && userInput.size() == 2){
        unsigned int index = std::stoull(userInput[1]); //TODO herausfinden wie UserInput zu enum

        return deletePart(index);
    }
    if (userInput.at(0) == "changePart" && userInput.size() == 1){
        return changePart();
    }
    if (userInput.at(0) == "formatPart" && userInput.size() == 4){
        unsigned int index = std::stoull(userInput[1]);
        unsigned long long size = std::stoull(userInput[2]); // TODO von wo krieg ich das?
        ByteSizes byteSize = Byte; //Todo von wo kriege ich das?
        SpeicherSystem speicherSystem = stringToSpeicherSystem(userInput.at(2));
        BlockSizes blockSize = BlockSizes::B_512; //Todo von wo krieg ich das?
        return formatPart(index,size,byteSize,speicherSystem,blockSize);
    }
    if (userInput.at(0) == "createFile" && userInput.size() == 4){
        std::string* name = &userInput.at(1);
        unsigned long long fileSize = std::stoull(userInput[2]);
        Flags flags = stringToFlags(userInput.at(3));
        return createFile(name,fileSize,flags);
    }
    if (userInput.at(0) == "listFiles" && userInput.size() == 1){
        return listFiles();
    }
    if (userInput.at(0) == "deleteFile" && userInput.size() == 2){
        std::string* name = &userInput.at(1);
        return deleteFile(name);
    }
     if (userInput.at(0) == "insertFile" && userInput.size() == 3){
        std::string* name = &userInput.at(1);
        std::string path = userInput.at(2);
        return insertFile(name,path);
     }
     if (userInput.at(0) == "readFile" && userInput.size() == 3){
         std::string* name = &userInput.at(1);
         std::string output = userInput[2];
         std::ostream& outputstream = std::cout << output;  //todo konvertieren
         return readFile(name, outputstream);
     }




    std::cout<< "No valid command!" << std::endl;

    return false;
}

std::string mapMbrToString(MBR* mbr, size_t curPartIndex) {
    if (mbr == nullptr) return "";
    using namespace std;
    string output = "";
    const string BORDER= "|";
    struct PartitionSize {
        size_t partIndex;
        float usedPercentage;
    };
    vector<PartitionSize> partSizes;
    partSizes.reserve(mbr->getPartitionCount());
    for (size_t i = 0; i < MBR::MAX_PARTITION_COUNT; i++) {
        if (mbr->getPartitions()[i].firstSektor == nullptr || mbr->getPartitions()[i].system == nullptr) {
            continue;
        }
        const size_t ds = mbr->getPartitions()[i].system->DRIVE_SIZE;
        const size_t freeSpace = mbr->getPartitions()[i].system->getFreeSpace();
        PartitionSize ps = {i, static_cast<float>(ds - freeSpace) / ds};
        partSizes.push_back(ps);
    }
    output.reserve(7 * (MBR::MAX_PARTITION_COUNT * 5)); //Reserve the space for the string
    float tolerance = 0.1f;
    vector<size_t> wasPaintedVec;
    wasPaintedVec.reserve(partSizes.size());
    const size_t MAX_LINE_HEIGHT = 7;
    for (size_t lineHeight = 0; lineHeight < MAX_LINE_HEIGHT; lineHeight++) {
        size_t percentageFull = (MAX_LINE_HEIGHT - lineHeight) / 7.0f;
        size_t curIndex = 0;
        for (PartitionSize ps : partSizes) {
            if (curIndex == curPartIndex) {
                output.append(colorize(BORDER, Color::YELLOW));
            } else {
                output.append(BORDER);
            }

            bool wasPainted = false;
            for (size_t i : wasPaintedVec) {
                if (i == curIndex) {
                    wasPainted = true;
                    break;
                }
            }
            if (std::abs(ps.usedPercentage - percentageFull) <= tolerance && !wasPainted) {
                output.append(colorize("----------", Color::CYAN));
                wasPaintedVec.push_back(curIndex);
            } else {
                output.append("          ");
            }

            if (curIndex == curPartIndex) {
                output.append(colorize(BORDER, Color::YELLOW));
            } else {
                output.append(BORDER);
            }
            curIndex++;
        }
        output.append("\n");
    }
    return output;
}

void MainWindow::loadDrive() {
    this->renderedView->clear();
    if (this->drives.size() == 0) {
        this->renderedView->setText("Empty...");
        return;
    }
    using namespace std;
    string output = "Drive No: ";
    output.append(to_string(mbrIndex));
    output.append("\n");
    if (this->drives.at(mbrIndex)->getPartitionCount() > 0)
        output.append(mapMbrToString(this->drives.at(mbrIndex), partIndex));
    this->renderedView->setHtml(QString::fromStdString(convertToHtmlWithColors(output)));
}

bool MainWindow::createDisk(size_t size, ByteSizes byteSize) {
  unsigned long long driveSize = size * getSizeInByte(byteSize);
  MBR* mbr = new MBR(driveSize);
  this->drives.push_back(mbr);
  return true;
}

bool MainWindow::deleteDisk(size_t index) {
  if (index > drives.size()) {
    std::cout << colorize("Error: Cannot delete Drive: Invalid drive index!", Color::RED) << std::endl;
    return false;
  }
  drives.erase(drives.begin() + index);
  return true;
}
bool MainWindow::changeDisk() {
    const size_t driveCount = this->drives.size();
    if (driveCount == 0) {
        std::cout << colorize("Error: You have no drives, create one with createDisk.", Color::RED) << std::endl;
        return false;
    }
    if (this->drives.size() < ++mbrIndex)
        mbrIndex = 0;
    partIndex = 0;
    loadDrive();
    return true;
}

bool MainWindow::wipeDisk(size_t index) {
  if (index > this->drives.size()) {
    std::cout << colorize("Error: Cannot wipe Disk: Invalid drive index!", Color::RED) << std::endl;
    return false;
  }
  MBR* mbr = this->drives.at(index);
  for (size_t i = 0; i < MBR::MAX_PARTITION_COUNT; i++) {
    if (mbr->getPartitions()[i].firstSektor != nullptr) {
      mbr->deletePartition(i);
    }
  }
  return true;
}
bool MainWindow::createPart(size_t size, ByteSizes byteSize, SpeicherSystem system, BlockSizes blockSize) {
  return createPart(size * getSizeInByte(byteSize), system, blockSize);
}

bool MainWindow::createPart(unsigned long long size, SpeicherSystem system, BlockSizes blockSize) {
  if (mbrIndex >= this->drives.size()) {
    std::cout << colorize("Error: Internal Error, please try again!", Color::RED) << std::endl;
    mbrIndex = 0;
    return false;
  }
  MBR* mbr = this->drives.data()[mbrIndex];
  if (mbr->getPartitionCount() >= MBR::MAX_PARTITION_COUNT) {
    std::cout << colorize("Error: Cannot create Disk: Disk is full!", Color::RED) << std::endl;
    return false;
  }
  Partition* p = mbr->createPartition(size, system, blockSize);
  return p != nullptr;
}

bool MainWindow::deletePart(size_t index) {
  if (index > this->drives.size()){
    return false;
  }
  MBR* mbr = this->drives.at(mbrIndex);
  if (mbr->getPartitionCount() < index)
    return false;
  mbr->deletePartition(index);
  return true;
}

bool MainWindow::changePart() {
    if (this->drives.size() <= mbrIndex) {
        std::cout << colorize("Error: You have no drive to show any partitions.", Color::RED) << std::endl;
        return false;
    }
    const size_t partCount = this->drives.at(mbrIndex)->getPartitionCount();
    if (partCount == 0) {
        std::cout << colorize("Error: You have no partitions, create one with createPart.", Color::RED) << std::endl;
        return false;
    }
    if (partCount < ++partIndex)
        partIndex = 0;
    loadDrive();
    return true;
}

bool MainWindow::formatPart(size_t index, SpeicherSystem system, BlockSizes blockSize) {
  if (index > this->drives.size()) {
    std::cout << colorize("Error: Cannot format Partition: Index is invalid!", Color::RED) << std::endl;
    return false;
  }
  unsigned long long partSize = this->drives.at(mbrIndex)->getPartitions()[index].system->DRIVE_SIZE;
  if (!deletePart(index)) {
    std::cout << colorize("Error: Could not format Partition: Error in delete partition!", Color::MAGENTA) << std::endl;
    return false;
  }
  if (!createPart(partSize, system, blockSize)) {
    std::cout << colorize("Error could not format Partition: Error in creating partition!", Color::MAGENTA) << std::endl;
    return false;
  }
  return true;
}

bool MainWindow::createFile(std::string* name, unsigned long long fileSize, unsigned char flags) {
  if (mbrIndex >= this->drives.size()) {
    std::cout << colorize("Internal error, no files read! Please try again!", Color::RED) << std::endl;
    this->mbrIndex = 0;
    return false;
  }
  MBR* mbr = this->drives.at(mbrIndex);
  if (partIndex > mbr->getPartitionCount()) {
    std::cout << colorize("Internal error, no files read! Please try again!", Color::RED) << std::endl;
    this->partIndex = 0;
    return false;
  }
  System* system = mbr->getPartitions()[partIndex].system;
  if (system == nullptr) {
    std::cout << colorize("Error: Cannot create File without a filesystem! First create one with createPart!" , Color::RED) << std::endl;
    return false;
  }

  std::shared_ptr<File> file = system->createFile(name, 0, flags);
  if (file == nullptr) {
    return false;
  }
  return file->resizeFile(fileSize);
}

bool MainWindow::listFiles() {
  if (mbrIndex >= this->drives.size()) {
    std::cout << colorize("Internal error, no files read! Please try again!", Color::RED) << std::endl;
    this->mbrIndex = 0;
    return false;
  }
  MBR* mbr = this->drives.at(mbrIndex);
  if (partIndex > mbr->getPartitionCount()) {
    std::cout << colorize("Internal error, no files read! Please try again!", Color::RED) << std::endl;
    this->partIndex = 0;
    return false;
  }
  if (mbr->getPartitions()[partIndex].system == nullptr){
    std::cout << colorize("No files found. Partition is not formatted.", Color::MAGENTA) << std::endl;
    return true;
  }
  System* system = mbr->getPartitions()[partIndex].system;
  std::vector<std::shared_ptr<File>> files = system->getAllFiles();
  for (std::shared_ptr<File>& f : files) {
    std::cout << "File \"" << colorize(*(f->getFilePath()), Color::YELLOW) << "\" Flags =" << f->getFlags() << "\n";
  }
  std::cout.flush();
  return true;
}

bool MainWindow::deleteFile(std::string* name) {
  if (mbrIndex >= this->drives.size()) {
    std::cout << colorize("Internal error, no files deleted!", Color::RED) << std::endl;
    this->mbrIndex = 0;
    return false;
  }
  MBR* mbr = this->drives.at(mbrIndex);
  if (partIndex > mbr->getPartitionCount()) {
    std::cout << colorize("Internal error, no files deleted!", Color::RED) << std::endl;
    this->partIndex = 0;
    return false;
  }
  return this->drives.at(mbrIndex)->getPartitions()[partIndex].system->deleteFile(name);
}

bool MainWindow::insertFile(std::string* name, std::string pathToCopyFrom) {
    if (mbrIndex >= this->drives.size()) {
        std::cout << colorize("Internal error, Could not find file in system!", Color::RED) << std::endl;
        this->mbrIndex = 0;
        return false;
    }
    MBR* mbr = this->drives.at(mbrIndex);
    if (partIndex >= mbr->getPartitionCount()) {
        std::cout << colorize("Internal error, Could not find file in system!", Color::RED) << std::endl;
        this->partIndex = 0;
        return false;
    }
    System* system = mbr->getPartitions()[partIndex].system;
    if (system == nullptr) {
        std::cout << colorize("Error: Cannot insert into File without a filesystem! First create one with createPart and then a file!" , Color::RED) << std::endl;
        return false;
    }
    using namespace std;

    shared_ptr<File> internFile = system->getFile(name);
    if (internFile == nullptr) {
        cout << colorize("Error: No file with that name found in system! Please create one first with createFile!", Color::RED) << endl;
        return false;
    }

    size_t filelen = 0;

    try {
      filelen = filesystem::file_size(pathToCopyFrom);
    } catch (std::filesystem::filesystem_error& ex) {
      std::cout << colorize("Error: Cannot find input-file!", Color::RED) << std::endl;
      return false;
    } catch (std::bad_alloc& ex) {
      std::cout << colorize("Error: Cannot find input-file!", Color::RED) << std::endl;
      return false;
    }

    if (!internFile->resizeFile(filelen)) {
        cout << colorize("Error: Cannot resize File!", Color::RED) << endl;
        return false;
    }

    ifstream inputFile(pathToCopyFrom);

    if(!inputFile.is_open()) {
        cout << colorize("Error: Cannot open input-file!", Color::RED) << endl;
        return false;
    }

    Array* arr = new Array(filelen);
    if (arr == nullptr) {
        cout << colorize("Error: Cannot save file in fs!", Color::RED) << endl;
        return false;
    }

    string line;
    unsigned char* curPtr = arr->getArray();
    while (getline(inputFile, line)) {
        strncpy(reinterpret_cast<char*>(curPtr), line.c_str(), line.size());
    }

    inputFile.close();

    if (!internFile->setData(arr)) {
        cout << colorize("Error: Cannot save file in fs!", Color::RED) << endl;
    }

    delete arr;
    return true;
}

bool MainWindow::readFile(std::string* name, std::ostream& outputStream) {
    if (mbrIndex >= this->drives.size()) {
            std::cout << colorize("Internal error, Could not find file in system!", Color::RED) << std::endl;
            this->mbrIndex = 0;
            return false;
        }
        MBR* mbr = this->drives.at(mbrIndex);
        if (partIndex >= mbr->getPartitionCount()) {
            std::cout << colorize("Internal error, Could not find file in system!", Color::RED) << std::endl;
            this->partIndex = 0;
            return false;
        }
        System* system = mbr->getPartitions()[partIndex].system;
        if (system == nullptr) {
            std::cout << colorize("Error: Cannot insert into File without a filesystem! First create one with createPart and then a file!" , Color::RED) << std::endl;
            return false;
        }
        using namespace std;

        shared_ptr<File> internFile = system->getFile(name);
        if (internFile == nullptr) {
            cout << colorize("Error: No file with that name found in system! Please create one first with createFile!", Color::RED) << endl;
            return false;
        }


        unique_ptr<Array> arr = internFile->getData();
        if (arr == nullptr) {
            cout << colorize("Error: Cannot save file in fs!", Color::RED) << endl;
            return false;
        }

        for (size_t i = 0; i < arr->getLength(); i++) {
            outputStream << (arr->getArray() + i);
        }
        outputStream.flush();

        arr.reset();
        return true;
}

void MainWindow::setCommandHints() {
  cmds.clear();
  cmds.push_back(Command::EXIT);
  cmds.push_back(Command::DISK_CREATE);
  if (this->drives.size() == 0) {
    setHelpCommands(cmds);
    return;
  }
  cmds.push_back(Command::DISK_WIPE);
  cmds.push_back(Command::DISK_DELETE);
  cmds.push_back(Command::DISK_CHANGE);
  if (mbrIndex >= this->drives.size())
    mbrIndex = 0;
  MBR* curMbr = this->drives.at(mbrIndex);
  size_t PART_COUNT = curMbr->getPartitionCount();
  if (PART_COUNT < MBR::MAX_PARTITION_COUNT) {
    cmds.push_back(Command::PARTITION_CREATE);
  }
  if (partIndex > PART_COUNT)
    partIndex = 0;
  if (PART_COUNT > 0) {
    cmds.push_back(Command::PARTITION_FORMAT);
    cmds.push_back(Command::PARTITION_CHANGE);
    cmds.push_back(Command::PARTITION_DELETE);
  }
  if (curMbr->getPartitions()[0].system == nullptr) {
    setHelpCommands(cmds);
    return;
  }
  cmds.push_back(Command::PARTITION_GET_FRAGMENTATION);
  cmds.push_back(Command::PARTITION_DEFRAGMENT);
  size_t FILE_COUNT = curMbr->getPartitions()[0].system->getFileCount();
  size_t FREE_SPACE = curMbr->getPartitions()[0].system->getFreeSpace();
  cmds.push_back(Command::FILE_LIST);
  if (FREE_SPACE > 0) {
    cmds.push_back(Command::FILE_CREATE);
  }
  if (FILE_COUNT > 0) {
    cmds.push_back(Command::FILE_DELETE);
    cmds.push_back(Command::FILE_INSERT);
    cmds.push_back(Command::FILE_READ);
  }

  setHelpCommands(cmds);
}
