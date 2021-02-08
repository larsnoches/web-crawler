#include "page.h"
#include "util.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <iterator>

using namespace std;

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

bool Page::extractLinkFromData(std::string& str, std::string& href)
{
    string hrefValStart = "href=\"";
    string hrefValEnd = "\"";
//    string href = hrefRecur;
    if (href.empty())
    {
        size_t pos1 = str.find(hrefValStart, 0);
        if (pos1 != string::npos)
        {
            size_t copyPos1 = pos1 + + hrefValStart.length();
            size_t pos2 = str.find(hrefValEnd, copyPos1);
            if (pos2 == string::npos)
            {
                href = str.substr(copyPos1, str.length() - copyPos1);
                return false;
            }
            href = str.substr(copyPos1, pos2 - copyPos1);
            Util::trim(href);
            str.erase(0, pos2 + hrefValEnd.length());
        }
    }
    else
    {
        size_t pos1 = str.find(hrefValEnd, 0);
        if (pos1 == string::npos)
        {
            href += str.substr(0, str.length());
            return false;
        }
        href += str.substr(0, pos1);
        Util::trim(href);
        str.erase(0, pos1 + hrefValEnd.length());
    }
    if (str.empty()) return false;
    if (href.empty()) return false;
    return true;
}

deque<string> Page::findLinks()
{
    deque<string> links;
    string currentHref;

    std::istringstream iss(m_data);
    int dataLen = m_data.length();
    int dataReadedLen = 0;

    while (dataReadedLen < dataLen)
    {
        int readingLen = 256;
        if (dataReadedLen + readingLen > dataLen)
        {
            readingLen = dataLen - dataReadedLen;
        }

        char line[readingLen];
        iss.read(line, readingLen);

        // search link
        string lineStr = line;
        cout << "line:" << lineStr << endl << endl;
        while (extractLinkFromData(lineStr, currentHref))
        {
            cout << "href:" << currentHref << endl << endl;
            links.push_back(currentHref);
            currentHref.clear();
        }
        // replace link: m_data.replace()

//        if (!href.empty()) links.push_back(href);

        dataReadedLen += readingLen;
    }


    return links;
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
