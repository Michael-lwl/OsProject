#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include "./command.h"
#include "./../../../include/utils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QWidget {
public:
    MainWindow(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // Create the renderedView (50% height, 100% width)
        QTextEdit* renderedView = new QTextEdit("*ToBeContinued*");
        renderedView->setMinimumHeight(sizeHint().height() * 0.5);
        mainLayout->addWidget(renderedView);

        // Create the bottomGroup (remaining space)
        QSplitter* bottomGroup = new QSplitter(Qt::Horizontal);

        // textGroup (70% width)
        QWidget* textGroup = new QWidget;
        QVBoxLayout* textGroupLayout = new QVBoxLayout(textGroup);

        // logView (80% height, 100% width)
        logView = new QTextEdit("Welcome to our Project: A Drive-Simulator");
        logView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        const Qt::TextInteractionFlags textFlags = Qt::TextInteractionFlag::TextSelectableByMouse
                                     | Qt::TextInteractionFlag::LinksAccessibleByMouse
                                     | Qt::TextInteractionFlag::TextSelectableByKeyboard
                                     | Qt::TextInteractionFlag::LinksAccessibleByKeyboard;
        logView->setTextInteractionFlags(textFlags);
        textGroupLayout->addWidget(logView, 8);

        // Horizontal layout for the command input field and button
        QWidget* commandInputWidget = new QWidget;
        QHBoxLayout* commandInputLayout = new QHBoxLayout(commandInputWidget);

        // commandInput (command input text field)
        commandInput = new QLineEdit();
        commandInput->setPlaceholderText("Enter command...");
        commandInputLayout->addWidget(commandInput);

        // Command button next to the command input field (square button)
        commandButton = new QPushButton("Send");
        commandButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        commandInputLayout->addWidget(commandButton);

        // Adjust size to make the button square without using resizeEvent
        QObject::connect(commandButton, &QPushButton::clicked, this, [this]() {
            int height = commandButton->height();
            commandButton->setFixedWidth(height);
        });

        // Capture '\n’ as send command
        QObject::connect(commandInput, &QLineEdit::returnPressed, this, [this]() {
            // Handle the Enter/Return key press here.
            QString command = commandInput->text();
            // Clear the input after handling the command
            if (handleCommand(command.toStdString()))
                commandInput->clear();
        });

        textGroupLayout->addWidget(commandInputWidget, 2);

        bottomGroup->addWidget(textGroup);
        textGroup->setLayout(textGroupLayout);

        // scrollableCommandView (remaining space, commands as buttons)

        QWidget* scrollableCommandView = new QWidget;
        QVBoxLayout* scrollableCommandViewLayout = new QVBoxLayout(scrollableCommandView);

        // Add a scrollable area for the buttons
        QScrollArea* scrollArea = new QScrollArea;
        QWidget* buttonContainer = new QWidget;
        buttonLayout = new QVBoxLayout(buttonContainer);

        //Init Log window
        Output::os = new std::ostringstream();
        std::vector<Command> cs;
        cs.push_back(Command::EXIT);
        setHelpCommands(cs);

        buttonContainer->setLayout(buttonLayout);
        scrollArea->setWidget(buttonContainer);
        scrollArea->setWidgetResizable(true);

        scrollableCommandViewLayout->addWidget(scrollArea);
        scrollableCommandView->setLayout(scrollableCommandViewLayout);
        bottomGroup->addWidget(scrollableCommandView);

        // Set proportions for bottomGroup
        bottomGroup->setStretchFactor(0, 7);  // textGroup takes 70%
        bottomGroup->setStretchFactor(1, 3);  // scrollableCommandView takes remaining space (30%)

        mainLayout->addWidget(bottomGroup);

        // Set the layout to the main window
        setLayout(mainLayout);
    }

    void setHelpCommands(std::vector<Command> commands);


protected:
    void resizeEvent(QResizeEvent* event) override {
        // Ensure the button remains square
        int height = this->commandButton->height();
        commandButton->setFixedWidth(height);

        QWidget::resizeEvent(event);
    }

    void setCommand(Command c);

    bool handleCommand(str command);

private:
    QTextEdit* logView;
    QPushButton* commandButton;
    QVBoxLayout* buttonLayout;
    QLineEdit* commandInput;
};


#endif // MAINWINDOW_H
