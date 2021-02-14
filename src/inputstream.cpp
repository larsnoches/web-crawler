/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#include "inputstream.h"

#include <string.h>
#include "ioexception.h"

using namespace std;

InputStream::InputStream()
    : eof(false)
{
}

bool InputStream::waitUnbuffered(int timeout)
{
    return true;
}

int InputStream::fillBuffer()
{
    if (eof) return 0;
    char b[1024];
    int br = readSomeUnbuffered(b, sizeof(b));
    if (br == 0)
    {
        eof = true;
    }
    else
    {
        buffer.append(b, br);
    }
    return br;
}

void InputStream::fillBuffer(int len)
{
    while (buffer.length() < static_cast<size_t>(len))
    {
        int br = fillBuffer();
        if (br == 0) throw IOException("Unexpected end of stream");
    }
}

bool InputStream::isEof()
{
    return (eof) && (buffer.empty());
}

int InputStream::readSomeFromBuffer(void* buf, int len)
{
    if (buffer.empty()) return 0;
    int l = min<int>(buffer.length(), len);
    memcpy(buf, buffer.c_str(), l);
    buffer.erase(0, l);
    return l;
}

string InputStream::readSome()
{
    if (buffer.empty()) fillBuffer();
    string t = buffer;
    buffer.clear();
    return t;
}

int InputStream::readSome(void* buf, int len)
{
    if (buffer.empty()) fillBuffer();
    if (buffer.empty()) return 0;
    return readSomeFromBuffer(buf, len);
}

bool InputStream::wait(int timeout)
{
    if (!buffer.empty()) return true;
    return waitUnbuffered(timeout);
}

std::string InputStream::read(int len)
{
    fillBuffer(len);
    string st = buffer.substr(0, len);
    buffer.erase(0, len);
    return st;
}

void InputStream::read(void* buf, int len)
{
    fillBuffer(len);
    memcpy(buf, buffer.c_str(), len);
    buffer.erase(0, len);
}

bool InputStream::waitForLine(int timeout)
{
    int newline = buffer.find('\n');
    int pos = buffer.length();
    while (newline == (int)string::npos)
    {
        if (!waitUnbuffered(timeout)) return false;
        int br = fillBuffer();
        if (br == 0) break;
        newline = buffer.find('\n', pos);
        pos = buffer.length();
    }
    return true;
}

std::string InputStream::readLine()
{
    int newline = buffer.find('\n');
    int pos = buffer.length();
    while (newline == (int)string::npos)
    {
        int br = fillBuffer();
        if (br == 0) break;
        newline = buffer.find('\n', pos);
        pos = buffer.length();
    }
    string r;
    if (newline == (int)string::npos)
    {
        r = buffer;
        buffer.clear();
    }
    else
    {
        r = buffer.substr(0, newline);
        buffer.erase(0, newline + 1);
    }
    if ((!r.empty()) && (r[r.length() - 1] == '\r'))
    {
        r.resize(r.length() - 1);
    }
    return r;
}
