#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <string>

class InputStream
{
private:
    int fillBuffer();
    void fillBuffer(int len);
protected:
    bool eof; //Means end of data in the source, but NOT in the buffer.
    std::string buffer;
    int readSomeFromBuffer(void* buf,int len);
    virtual int readSomeUnbuffered(void* buf,int len)=0;
    virtual bool waitUnbuffered(int timeout);
public:
    InputStream();
    //Wait until input is available. Returns true if the stream is ready for reading.
    //You can specify 0 timeout to probe stream for data.
    //
    //Success (true returned) means that you can read some data with ReadSome() and it won't block.
    bool wait(int timeout);
    //Check for end of stream state.
    //No data from the source is available, and the buffer is empty.
    bool isEof();
    //Read some input.
    std::string readSome();
    //Read some input.
    int readSome(void* buf,int len);
    //Read specific number of bytes.
    std::string read(int len);
    //Read specific number of bytes. Adapted for reading binary objects.
    void read(void* buf,int len);
    //Wait until line can be read.
    bool waitForLine(int timeout);
    //Read line from the stream. Line ends with '\n'. If '\r' is also present, it is removed.
    std::string readLine();
};

#endif // INPUTSTREAM_H
