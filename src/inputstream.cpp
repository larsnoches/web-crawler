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
    : m_eof(false)
{
}

bool InputStream::waitUnbuffered(int timeout)
{
    return true;
}

int InputStream::fillBuffer()
{
    if (m_eof) return 0;
    char b[1024];
    int br = readSomeUnbuffered(b, sizeof(b));
    if (br == 0)
    {
        m_eof = true;
    }
    else
    {
        m_buffer.append(b, br);
    }
    return br;
}

void InputStream::fillBuffer(int len)
{
    while (m_buffer.length() < static_cast<size_t>(len))
    {
        int br = fillBuffer();
        if (br == 0) throw IOException("Unexpected end of stream");
    }
}

bool InputStream::isEof()
{
    return (m_eof) && (m_buffer.empty());
}

int InputStream::readSomeFromBuffer(void* buf, int len)
{
    if (m_buffer.empty()) return 0;
    int l = min<int>(m_buffer.length(), len);
    memcpy(buf, m_buffer.c_str(), l);
    m_buffer.erase(0, l);
    return l;
}

string InputStream::readSome()
{
    if (m_buffer.empty()) fillBuffer();
    string t = m_buffer;
    m_buffer.clear();
    return t;
}

int InputStream::readSome(void* buf, int len)
{
    if (m_buffer.empty()) fillBuffer();
    if (m_buffer.empty()) return 0;
    return readSomeFromBuffer(buf, len);
}

bool InputStream::wait(int timeout)
{
    if (!m_buffer.empty()) return true;
    return waitUnbuffered(timeout);
}

std::string InputStream::read(int len)
{
    fillBuffer(len);
    string st = m_buffer.substr(0, len);
    m_buffer.erase(0, len);
    return st;
}

void InputStream::read(void* buf, int len)
{
    fillBuffer(len);
    memcpy(buf, m_buffer.c_str(), len);
    m_buffer.erase(0, len);
}

bool InputStream::waitForLine(int timeout)
{
    int newline = m_buffer.find('\n');
    int pos = m_buffer.length();
    while (newline == (int)string::npos)
    {
        if (!waitUnbuffered(timeout)) return false;
        int br = fillBuffer();
        if (br == 0) break;
        newline = m_buffer.find('\n', pos);
        pos = m_buffer.length();
    }
    return true;
}

std::string InputStream::readLine()
{
    int newline = m_buffer.find('\n');
    int pos = m_buffer.length();
    while (newline == (int)string::npos)
    {
        int br = fillBuffer();
        if (br == 0) break;
        newline = m_buffer.find('\n', pos);
        pos = m_buffer.length();
    }
    string r;
    if (newline == (int)string::npos)
    {
        r = m_buffer;
        m_buffer.clear();
    }
    else
    {
        r = m_buffer.substr(0, newline);
        m_buffer.erase(0, newline + 1);
    }
    if ((!r.empty()) && (r[r.length() - 1] == '\r'))
    {
        r.resize(r.length() - 1);
    }
    return r;
}
