#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <string>

class OutputStream
{
public:
    //Write some output.
    //You should override this method in derived classes.
    //Return value is the actual number of bytes written.
    virtual int writeSome(const void* buf,int len)=0;
    //Write data to the stream.
    void write(const std::string& st);
    //Write data to the stream. Adapted for writing binary objects.
    void write(const void* buf,int len);
    //Write line of text to the stream. Differs from Write(string) only by adding '\n' after writing.
    void writeLine(const std::string& st);
};

#endif // OUTPUTSTREAM_H
