#include <QPlainTextEdit>
#include <sstream>
#include <iostream>
#include "./../utils.h"

#ifndef STREAM_BUF_H
#define STREAM_BUF_H


class CustomStreamBuf : public std::streambuf {
public:
    CustomStreamBuf(std::ostringstream& oss, QPlainTextEdit* textEdit)
        : oss(oss), textEdit(textEdit) {
        // Allocate a buffer to use for overflow
        setp(buffer, buffer + sizeof(buffer) - 1);
    }

    ~CustomStreamBuf() override = default;

protected:
    // This method is called when the internal buffer is full or needs to flush
    int overflow(int c = EOF) override {
        if (c != EOF) {
            // Check if the buffer is full
            if (pptr() == epptr()) {
                //flush the buffer
                sync();
            }
            // Add the character to the buffer
            *pptr() = c;
            pbump(1);
        }
        return 0; // Indicate success
    }

    // This method is called to flush the buffer
    int sync() override {
        // Get the current content of the buffer
        std::ptrdiff_t n = pptr() - pbase();
        if (n > 0) {
            // Write buffer content to ostringstream
            oss.write(pbase(), n);

            // Convert buffer content to HTML and update the QPlainTextEdit
            QString html = QString::fromStdString(convertToHtmlWithColors(oss.str()));
            textEdit->appendHtml(html);
            textEdit->moveCursor(QTextCursor::End);

            // Clear the ostringstream and reset the buffer
            oss.str("");
            oss.clear();
            setp(buffer, buffer + sizeof(buffer) - 1);
        }
        return 0;
    }

private:
    std::ostringstream& oss;
    QPlainTextEdit* textEdit;
    char buffer[1024 << 2];  // Internal buffer to store output before flushing
};
#endif //STREAM_BUF_H
