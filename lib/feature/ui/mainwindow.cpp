#include "./../../../include/feature/ui/mainwindow.h"
#include "./../../../include/utils.h"
#include <iostream>
#include <qt/QtWidgets/qlayout.h>
#include <qt/QtWidgets/qlayoutitem.h>
#include <qt/QtWidgets/qpushbutton.h>
#include <qt/QtWidgets/qwidget.h>
#include <cmath>
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
        if (mbr->getPartitions()[i].firstSektor == nullptr) {
            continue;
        }
        const size_t ds = mbr->getPartitions()[i].system->DRIVE_SIZE;
        const size_t freeSpace = mbr->getPartitions()[i].system->getFreeSpace();
        PartitionSize ps = {i, static_cast<float>(ds - freeSpace) / ds};
        partSizes.push_back(ps);
    }
    output.reserve(7 * (MBR::MAX_PARTITION_COUNT * 5)); //Reserve the space for the string
    float tolerance = 0.1f;
    for (size_t lineHeight = 0; lineHeight < 7; lineHeight++) {
        size_t percentageFull = (7.0f-lineHeight) / 7.0f;
        size_t curIndex = 0;
        for (PartitionSize ps : partSizes) {
            if (curIndex == curPartIndex) {
                output.append(colorize(BORDER, Color::YELLOW));
            } else {
                output.append(BORDER);
            }

            if (std::abs(ps.usedPercentage - percentageFull) <= tolerance) {
                output.append(colorize("----------", Color::CYAN));
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
    if (this->drives.at(mbrIndex)->getPartitionCount() != 0)
        output.append(mapMbrToString(this->drives.at(mbrIndex), partIndex));
    this->renderedView->setText(QString::fromStdString(output));
}

bool MainWindow::createDisk(unsigned long long size, ByteSizes byteSize) {}
bool MainWindow::deleteDisk(size_t index) {

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
bool MainWindow::wipeDisk(size_t index) {}
bool MainWindow::createPart(unsigned long long size, ByteSizes byteSize, SpeicherSystem system, BlockSizes blockSize) {}
bool MainWindow::deletePart(size_t index) {}
bool MainWindow::changePart() {
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
bool MainWindow::formatPart(size_t index, unsigned long long size, ByteSizes byteSize, SpeicherSystem system, BlockSizes blockSize ) {}
bool MainWindow::createFile(std::string* name, unsigned long long fileSize, unsigned char flags) {}
bool MainWindow::listFiles() {}
bool MainWindow::deleteFile(std::string* name) {}
bool MainWindow::insertFile(std::string* name, std::string pathToCopyFrom) {}
bool MainWindow::readFile(std::string* name, std::ostream& outputStream) {}
