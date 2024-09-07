#include <iostream>
#include <sstream>
#include <streambuf>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#ifndef STREAM_BUF_H
#define STREAM_BUF_H

class CustomStreamBuf : public std::streambuf {
public:
    CustomStreamBuf(std::ostringstream& oss, QTextEdit* textEdit)
        : oss(oss), textEdit(textEdit) {
        // Allocate a buffer to use for overflow
        setp(buffer, buffer + sizeof(buffer) - 1);
    }

    ~CustomStreamBuf() = default;

protected:
    // This method is called when the internal buffer is full or needs to flush
    int overflow(int c = EOF) override {
        if (c != EOF) {
            *pptr() = c;  // Add the character to the buffer
            pbump(1);  // Increment the put pointer
        }
        return sync() == 0 ? c : EOF;
    }

    // This method is called to flush the buffer
    int sync() override {
        // Get the current content of the buffer
        std::ptrdiff_t n = pptr() - pbase();
        if (n > 0) {
            // Write buffer content to ostringstream
            oss.write(pbase(), n);
            // Update the QTextEdit with the current content of the ostringstream
            textEdit->setPlainText(QString::fromStdString(oss.str()));
            //textEdit->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->maximum());

            // Reset the buffer
            setp(buffer, buffer + sizeof(buffer) - 1);
        }
        return 0;
    }

private:
    std::ostringstream& oss;
    QTextEdit* textEdit;
    char buffer[1024];  // Internal buffer to store output before flushing
};
#endif //STREAM_BUF_H
