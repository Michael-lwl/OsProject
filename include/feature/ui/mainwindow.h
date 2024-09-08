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
#include <QFont>
#include <QPalette>
#include <qt/QtCore/qglobal.h>
#include "./command.h"
#include "./../../utils.h"
#include "./../../core/stream_buf.h"
#include "./../../core/mbr.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QWidget {
public:

    ///Determins the dark mode if the given pallete's background is darker than its foreground
    static bool isDarkMode(const QPalette& palette);

    MainWindow(QWidget* parent = nullptr) : QWidget(parent),
        mbrIndex(0), partIndex(0)    {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        //Monospaced font
        QFont* textFont = new QFont;
        textFont->setFamily("monospace [Consolas]");
        textFont->setFixedPitch(true);
        textFont->setStyleHint(QFont::TypeWriter);
        //Not writeable text flags
        const Qt::TextInteractionFlags textFlags = Qt::TextInteractionFlag::TextSelectableByMouse
                                                   | Qt::TextInteractionFlag::LinksAccessibleByMouse
                                                   | Qt::TextInteractionFlag::TextSelectableByKeyboard
                                                   | Qt::TextInteractionFlag::LinksAccessibleByKeyboard;

        //change Highlight color depending on theme palette (light or dark)
        if(MainWindow::isDarkMode(QApplication::palette())) {
            this->highlightColor = Color::YELLOW;
        } else {
            this->highlightColor = Color::GREEN;
        }

        // Create the renderedView (50% height, 100% width)
        this->renderedView = new QTextEdit("*ToBeContinued*");
        this->renderedView->setFont(*textFont);
        this->renderedView->setMinimumHeight(sizeHint().height() * 0.5);
        this->renderedView->setTextInteractionFlags(textFlags);
        mainLayout->addWidget(this->renderedView);

        // Create the bottomGroup (remaining space)
        QSplitter* bottomGroup = new QSplitter(Qt::Horizontal);

        // textGroup (70% width)
        QWidget* textGroup = new QWidget;
        QVBoxLayout* textGroupLayout = new QVBoxLayout(textGroup);

        // logView (80% height, 100% width)
        this->logView = new QTextEdit("If you can read this, something has gone terribly wrong\n");
        this->logView->setFont(*textFont);
        this->logView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->logView->setTextInteractionFlags(textFlags);
        textGroupLayout->addWidget(this->logView, 8);

        //Redirect cout to our custom buffer
        this->customBuffer = new CustomStreamBuf(oss, logView);
        this->originalCoutBuffer = std::cout.rdbuf(customBuffer);  // Redirect std::cout to the custom buffer

        // Horizontal layout for the command input field and button
        QWidget* commandInputWidget = new QWidget;
        QHBoxLayout* commandInputLayout = new QHBoxLayout(commandInputWidget);

        // commandInput (command input text field)
        this->commandInput = new QLineEdit();
        this->commandInput->setPlaceholderText("Enter command...");
        commandInputLayout->addWidget(this->commandInput);

        // Command button next to the command input field (square button)
        this->commandButton = new QPushButton("↵");
        this->commandButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        commandInputLayout->addWidget(this->commandButton);

        // Adjust size to make the button square without using resizeEvent
        QObject::connect(this->commandButton, &QPushButton::clicked, this, [this]() {
            if (this->handleCommand(this->commandInput->text().toStdString()))
                this->commandInput->clear();
        });

        // Capture '\n’ as send command
        QObject::connect(this->commandInput, &QLineEdit::returnPressed, this, [this]() {
            // Handle the Enter/Return key press here.
            QString command = this->commandInput->text();
            // Clear the input after handling the command
            if (handleCommand(command.toStdString()))
                this->commandInput->clear();
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
        this->buttonLayout = new QVBoxLayout(buttonContainer);

        buttonContainer->setLayout(this->buttonLayout);
        scrollArea->setWidget(buttonContainer);
        scrollArea->setWidgetResizable(true);

        scrollableCommandViewLayout->addWidget(scrollArea);
        scrollableCommandView->setLayout(scrollableCommandViewLayout);
        bottomGroup->addWidget(scrollableCommandView);

        // Set proportions for bottomGroup
        bottomGroup->setStretchFactor(0, 6);  // textGroup takes 60%
        bottomGroup->setStretchFactor(1, 4);  // scrollableCommandView takes remaining space (40%)

        mainLayout->addWidget(bottomGroup);

        // Set the layout to the main window
        setLayout(mainLayout);
        std::cout << "Welcome to our Project: A Drive-Simulator" << std::endl;
        loadDrive();
        setCommandHints();
    }

    ~MainWindow() {
        //Set cout buffer to its original buffer!
        for (MBR* mbr : drives) {
            delete mbr;
        }
        std::cout.rdbuf(originalCoutBuffer);
        delete customBuffer;
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

    void loadDrive();

    bool createDisk(size_t size, ByteSizes byteSize);
    bool deleteDisk(size_t index);
    bool changeDisk();
    bool wipeDisk(size_t index);
    bool createPart(size_t size, ByteSizes byteSize, SpeicherSystem system, BlockSizes blockSize = BlockSizes::B_512);
    bool createPart(unsigned long long size, SpeicherSystem system, BlockSizes blockSize = BlockSizes::B_512);
    bool deletePart(size_t index);
    bool changePart();
    bool formatPart(size_t index, SpeicherSystem system, BlockSizes blockSize = BlockSizes::B_512);
    bool getFragmentPart(size_t partIndex);
    bool defragmentPart(size_t partIndex);
    bool createFile(std::string* name, unsigned long long fileSize, unsigned char flags = Flags::ASCII);
    bool listFiles();
    bool deleteFile(std::string* name);
    bool insertFile(std::string* name, std::string pathToCopyFrom);
    bool readFile(std::string* name, std::ostream& outputStream = std::cout);

private:
    std::vector<MBR*> drives;
    size_t mbrIndex;
    size_t partIndex;
    QTextEdit* renderedView;
    QTextEdit* logView;
    QPushButton* commandButton;
    QVBoxLayout* buttonLayout;
    QLineEdit* commandInput;
    std::ostringstream oss;
    CustomStreamBuf* customBuffer;
    std::streambuf* originalCoutBuffer;
    Color highlightColor;
    std::vector<Command> cmds;

    void updateAll() {
        loadDrive();
        setCommandHints();
    }

    void setCommandHints();
};


#endif // MAINWINDOW_H
