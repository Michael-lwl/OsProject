#include "./../../../include/feature/ui/mainwindow.h"
#include "./../../../include/utils.h"
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

bool MainWindow::isDarkMode(const QPalette& palette) {
    QColor backgroundColor = palette.color(QPalette::Window);
    QColor textColor = palette.color(QPalette::WindowText);
    return backgroundColor.lightness() < textColor.lightness();
}


void MainWindow::setCommand(Command c) {
    this->commandInput->clear();
    this->commandInput->insert(c.getCmdTemplate().c_str());
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

std::string toLower(std::string& in) {
    std::string input = in; // Kopiere den Originalstring
    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return input;
}
SpeicherSystem stringToSpeicherSystem(std::string in){
    std::string input = "";
    input = toLower(in);
    if(input == "bs_fat") return SpeicherSystem::BS_FAT;
    if(input == "inodesystem") return SpeicherSystem::INODE_SYSTEM;
    std::cout << colorize( "Das eingegebene System wurde nicht erkannt. Es wird nun der Standardwert BS_Fat übergeben", Color::RED) << std::endl;
    return SpeicherSystem::BS_FAT;
}
ByteSizes stringToByteSize(std::string in){
    std::string input = toLower(in);
    if(input == "byte") return ByteSizes::Byte;
    if(input == "kilobyte") return ByteSizes::Kilobyte;
    if(input == "kb") return ByteSizes::KB;
    if(input == "kibibyte") return ByteSizes::Kibibyte;
    if(input == "kib") return ByteSizes::KiB;
    if(input == "megabyte") return ByteSizes::Megabyte;
    if(input == "mb") return ByteSizes::MB;
    if(input == "mebibyte") return ByteSizes::Mebibyte;
    if(input == "mib") return ByteSizes::MiB;
    if(input == "gigabyte") return ByteSizes::Gigabyte;
    if(input == "gb") return ByteSizes::GB;
    if(input == "gibibyte") return ByteSizes::Gibibyte;
    if(input == "gib") return ByteSizes::GiB;
    if(input == "terrabyte") return ByteSizes::Terrabyte;
    if(input == "tb") return ByteSizes::TB;
    if(input == "tebibyte") return ByteSizes::Tebibyte;
    if(input == "tib") return ByteSizes::TiB;
    std::cout << colorize( "Es wurde kein gültiger ByteSize-Wert übergeben, es wird nun mit der Standardgroesse Byte weitergearbeiter", Color::RED)  << std::endl;
    return ByteSizes::Byte;
};

BlockSizes stringToBlockSize(std::string in){
    std::string input = "";
    input = toLower(in);
    if(input == "b_512") return BlockSizes::B_512;
    if(input == "kib_1") return BlockSizes::KIB_1;
    if(input == "kib_2") return BlockSizes::KIB_2;
    if(input == "kib_4") return BlockSizes::KIB_4;
    if(input == "kib_8") return BlockSizes::KIB_8;
    if(input == "kib_16") return BlockSizes::KIB_16;
    std::cout << colorize( "Es wurde keine gültige BlockSize übergeben, es wird nun mit der Standardgroesse von B_512 weitergearbeitet", Color::RED)  << std::endl;
    return BlockSizes::B_512;
}

bool flagsAreValid(unsigned char input) {
    return (input != 30);
}
bool MainWindow::handleCommand(str command) {
    //Fast exit
    if (command.compare(Command::EXIT.getCmd()) == 0) {
        QCoreApplication::quit();
        return true;
    }
    std::vector<str> userInput = splitAt(&command, ' ');
    if (userInput.empty()) return false;

    if (userInput.at(0) == Command::DISK_CREATE.getCmd() && userInput.size() == 3) {
        try {
            unsigned long long size = std::stoull(userInput[1]);
            ByteSizes byteSize = stringToByteSize(userInput[2]);
            if (createDisk(size, byteSize)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim erstellen der Platte/des MBRs " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::DISK_DELETE.getCmd() && userInput.size() == 2) {
        try {
            unsigned int index = std::stoull(userInput[1]);
            if (deleteDisk(index)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim löschen der Platte " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::DISK_CHANGE.getCmd() && userInput.size() == 1) {
        try {
            if (changeDisk()) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim wechseln der Platte"  << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::DISK_WIPE.getCmd() && userInput.size() == 2) {
        try {
            unsigned int index = std::stoull(userInput[1]);
            if (wipeDisk(index)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler die Partitionen von der Platte zu werfen  " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::PARTITION_CREATE.getCmd() && userInput.size() == 5) {
        try {
            unsigned long long size = std::stoull(userInput[1]);
            ByteSizes byteSize = stringToByteSize(userInput.at(2));
            SpeicherSystem system = stringToSpeicherSystem(userInput.at(3));
            BlockSizes blockSize = stringToBlockSize(userInput.at(4));
            if (createPart(size, byteSize, system, blockSize)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler bei der Erstellung einer Partition" << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::PARTITION_DELETE.getCmd() && userInput.size() == 2) {
        try {
            unsigned int index = std::stoull(userInput[1]);
            if (deletePart(index)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim Löschen der Partition " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::PARTITION_CHANGE.getCmd() && userInput.size() == 1) {
        try {
            if (changePart()) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim wechseln der Partition " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::PARTITION_FORMAT.getCmd() && userInput.size() == 4) {
        try {
            unsigned int index = std::stoull(userInput[1]);
            SpeicherSystem speicherSystem = stringToSpeicherSystem(userInput.at(2));
            BlockSizes blockSize = BlockSizes::B_512;
            if (formatPart(index, speicherSystem, blockSize)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim formatieren der Partition: " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::FILE_CREATE.getCmd() && userInput.size() == 4) {
        try {
            std::string* name = &userInput.at(1);
            unsigned long long fileSize = std::stoull(userInput[2]);
            unsigned char flags = userInput.at(3)[0];
            if (!flagsAreValid(flags)) {
                std::cout << colorize("Error: not a valid flag!", Color::RED) << std::endl;
                return false;
            }
            if (createFile(name, fileSize, flags - 30)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim erstellen der Datei" << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::FILE_LIST.getCmd() && userInput.size() == 1) {
        try {
            if (listFiles()) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler bei der Darstellung der erstellten Dateien " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::FILE_DELETE.getCmd() && userInput.size() == 2) {
        try {
            std::string* name = &userInput.at(1);
            if (deleteFile(name)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim löschen der Dateien " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::FILE_INSERT.getCmd() && userInput.size() == 3) {
        try {
            std::string* name = &userInput.at(1);
            std::string path = userInput.at(2);
            if (insertFile(name, path)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim einfügen der Datei " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == Command::FILE_READ.getCmd() && 2 <= userInput.size() && userInput.size() <= 3) {
        try {
            std::string* name = &userInput.at(1);
            std::ostream* outputstream;
            if (userInput.size() == 3) {
                outputstream = new std::ofstream(userInput.at(2));
            } else {
                outputstream = &std::cout;
            }
            if (readFile(name, *outputstream)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim lesen der Datei " << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == "defragPart" && userInput.size() == 2) {
        try {
            size_t index = std::stoull(userInput[1]);
            if (defragmentPart(index)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim Defragmentieren" << e.what() << std::endl;
        }
        return false;
    }

    if (userInput.at(0) == "getFragmentation" && userInput.size() == 2) {
        try {
            size_t index = std::stoull(userInput[1]);
            if (getFragmentPart(index)) {
                loadDrive();
                setCommandHints();
                std::cout << colorize(command, Color::GREEN) << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler die Fragmentierung konnte nicht geholt werden " << e.what() << std::endl;
        }
        return false;
    }

    std::cout << "Invalid command!" << std::endl;
    return false;
}

std::string mapMbrToString(MBR* mbr, size_t curPartIndex, Color highlightColor) {
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
        if (mbr->getPartition(i) == nullptr || mbr->getPartition(i)->firstSektor == nullptr || mbr->getPartition(i)->system == nullptr) {
            continue;
        }
        const size_t ds = mbr->getPartition(i)->system->DRIVE_SIZE;
        const size_t freeSpace = mbr->getPartition(i)->system->getFreeSpace();
        PartitionSize ps = {i, static_cast<float>(ds - freeSpace) / ds};
        partSizes.push_back(ps);
    }
    output.reserve(7 * (MBR::MAX_PARTITION_COUNT * 5)); //Reserve the space for the string
    float tolerance = 0.1f;
    vector<size_t> wasPaintedVec;
    wasPaintedVec.reserve(partSizes.size());
    const size_t MAX_LINE_HEIGHT = 7;
    for (long lineHeight = MAX_LINE_HEIGHT - 1; 0 <= lineHeight; lineHeight--) {
        float percentageFull = (MAX_LINE_HEIGHT - lineHeight) / 7.0f;
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
                output.append(colorize(BORDER, highlightColor));
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
        output.append(mapMbrToString(this->drives.at(mbrIndex), partIndex, this->highlightColor));
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
    if (this->mbrIndex == index) {
        this->mbrIndex = 0;
    }
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
        if (mbr->getPartition(i)->firstSektor != nullptr) {
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

bool MainWindow::getFragmentPart(size_t partIndex) {
    if (mbrIndex >= this->drives.size()) {
        std::cout << colorize("Internal error! Please try again!", Color::RED) << std::endl;
        this->mbrIndex = 0;
        return false;
    }
    MBR* mbr = this->drives.at(mbrIndex);
    if (partIndex > mbr->getPartitionCount()) {
        std::cout << colorize("Internal error! Please try again!", Color::RED) << std::endl;
        this->partIndex = 0;
        return false;
    }
    System* system = mbr->getPartitions()[partIndex].system;
    if (system == nullptr) {
        std::cout << colorize("Error: Cannot get Fragmentation without a filesystem! First create one with createPart!" , Color::RED) << std::endl;
        return false;
    }
    std::cout << colorize("Fragmentation: " +std::to_string(system->getFragmentation()), Color::MAGENTA) << std::endl;
    return true;
}
bool MainWindow::defragmentPart(size_t partIndex) {
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
    system->show();
    bool output = system->defragDisk();
    system->show();
    return output;
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
    std::shared_ptr<File> file = system->createFile(name, fileSize, flags);
    return file != nullptr;
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
        std::cout << "File \"" << colorize(*(f->getFilePath()), Color::YELLOW) << "\" Flags =" << static_cast<int>(f->getFlags()) << "\n";
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

    std::cout << "Reading and printing the file. This can take a while..." << std::endl;
    for (size_t i = 0; i < arr->getLength(); i++) {
        outputStream << static_cast<int>(*(arr->getArray() + i));
    }
    outputStream << std::endl;

    std::cout << "Finished!" << std::endl;

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
    if (this->drives.size() > 1) {
        cmds.push_back(Command::DISK_CHANGE);
    }
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
        cmds.push_back(Command::PARTITION_DELETE);
    }
    if (PART_COUNT > 1){
        cmds.push_back(Command::PARTITION_CHANGE);
    }
    if (curMbr->getPartition(partIndex) == nullptr || curMbr->getPartition(partIndex)->system == nullptr) {
        setHelpCommands(cmds);
        return;
    }
    cmds.push_back(Command::PARTITION_GET_FRAGMENTATION);
    cmds.push_back(Command::PARTITION_DEFRAGMENT);
    size_t FILE_COUNT = curMbr->getPartition(partIndex)->system->getFileCount();
    size_t FREE_SPACE = curMbr->getPartition(partIndex)->system->getFreeSpace();
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
