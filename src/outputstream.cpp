/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#include "outputstream.h"

using namespace std;

void OutputStream::write(const void* buf, int len)
{
    const char* t = reinterpret_cast<const char*>(buf);
    int bytesWritten;
    while (len > 0)
    {
        bytesWritten = writeSome(t, len);
        t += bytesWritten;
        len -= bytesWritten;
    }
}

void OutputStream::write(const std::string& st)
{
    write(st.c_str(), st.length());
}

void OutputStream::writeLine(const std::string& st)
{
    write(st);
    write("\n");
}
