/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <string>

class OutputStream
{
public:
    // write some output;
    // you should override this method in derived classes,
    // return value is the actual number of bytes written
    virtual int writeSome(const void* buf, int len) = 0;
    // write data to the stream
    void write(const std::string& st);
    // write data to the stream, adapted for writing binary objects
    void write(const void* buf, int len);
    // write line of text to the stream, differs from write(string) only by adding '\n' after writing
    void writeLine(const std::string& st);
};

#endif // OUTPUTSTREAM_H
