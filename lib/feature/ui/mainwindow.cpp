#include "./../../../include/feature/ui/mainwindow.h"
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
    for (Command c : commands) {
        QPushButton* btn = new QPushButton(c.getName().c_str());
        QObject::connect(btn, &QPushButton::clicked, this, [this, c]() {
            this->setCommand(c);
        });
        btn->setToolTip(c.getDesc().c_str());
        this->buttonLayout->addWidget(btn);
    }
}

bool MainWindow::handleCommand(str command) {
    if (command.compare(Command::EXIT.getCmd()) == 0) {
        QCoreApplication::quit();
        return true;
    }
    std::cout << "No viable command!" << std::endl;

    return false;
}
