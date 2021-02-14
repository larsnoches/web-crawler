/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <string>

class InputStream
{
private:
    int fillBuffer();
    void fillBuffer(int len);
protected:
    bool eof; // means end of data in the source, but NOT in the buffer
    std::string buffer;
    int readSomeFromBuffer(void* buf,int len);
    virtual int readSomeUnbuffered(void* buf,int len)=0;
    virtual bool waitUnbuffered(int timeout);
public:
    InputStream();
    // wait until input is available; returns true if the stream is ready for reading,
    // you can specify 0 timeout to probe stream for data,
    // success (true returned) means that you can read some data with readSome() and it won't block
    bool wait(int timeout);
    // check for end of stream state
    // no data from the source is available, and the buffer is empty
    bool isEof();
    // read some input
    std::string readSome();
    // read some input
    int readSome(void* buf,int len);
    // read specific number of bytes
    std::string read(int len);
    // read specific number of bytes, adapted for reading binary objects
    void read(void* buf,int len);
    // wait until line can be read
    bool waitForLine(int timeout);
    // read line from the stream, line ends with '\n', if '\r' is also present, it is removed
    std::string readLine();
};

#endif // INPUTSTREAM_H
