#include "./../../../include/feature/ui/mainwindow.h"
#include "./../../../include/utils.h"
#include <iostream>
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
