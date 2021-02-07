#include "page.h"

//#include <iostream>
#include <fstream>
//#include <sstream>
#include <ios>
#include <iterator>
//#include <filesystem>

using namespace std;
//namespace fs = filesystem;

Page::Page()
{
    //
}

void Page::setName(string& name)
{
    m_name = name;
}

void Page::setPath(string& path)
{
    m_path = path;
}

void Page::setFakeName(string& fakeName)
{
    m_fakeName = fakeName;
}

void Page::writeData(std::string& line)
{
//    m_stream << line;
//    m_data.
//    std::copy(line.begin(), line.end(), std::back_inserter<std::vector<char> >(m_data));
    m_data += line;
}

void Page::save()
{
    if (m_fakeName.empty()) return;

    fstream outf("saved/" + m_fakeName + ".html", ios::out);
//    fstream outf(m_path + m_name, ios::out);

//    size_t size = m_data.size();
//    outf.write((char*)&size, sizeof(size));


    outf.write(m_data.data(), m_data.size());

//    ostream_iterator<std::string> outputIterator(outf, "\n");
//    std::copy(m_data.begin(), m_data.end(), outputIterator);

//    string data = m_stream.str();
//    int readedLen = 0;
//    int len = m_data.size();

//    while (readedLen < len)
//    {
//        int nextPos = readedLen + 1024;
//        if (nextPos > len) nextPos = len;
//        int toReadLen = nextPos - readedLen;

//        outf.write(data.data(), toReadLen);
//        readedLen = nextPos;
//    }
    outf.close();
}

string Page::getName()
{
    return m_name;
}

string Page::getPath()
{
    return m_path;
}

string Page::getFakeName()
{
    return m_fakeName;
}

string Page::readData()
{
    return m_data;
//    return m_data.data();
}

string Page::getLink()
{
    return m_path + m_name;
}

void Page::replaceLink(string& link)
{
//    string href;

//    if (href.empty())
//    {

//        int pos1 = str.find("href=\"", 0);
//        if (pos1 != string::npos)
//        {
//            int pos2 = str.find("\"", pos1);
//            if (pos2 == string::npos)
//            {
//                href = str.substr(pos1, str.length());
//            }
//            else
//            {
//                href = str.substr(pos1, pos2 - pos1);
//            }
//        }
//    }
//    else
//    {

//        int pos1 = str.find("\"", 0);
//        if (pos1 != string::npos)
//        {
//            href += str.substr(0, pos1);
//        }
//        else
//        {
//            href += str.substr(0, str.length())
//        }
//    }
}
