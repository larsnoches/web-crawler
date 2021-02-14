/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#include "page.h"
#include "util.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <iterator>

using namespace std;

Page::Page(string& saveFolder, int level)
    : m_saveFolder(saveFolder),
      m_level(level)
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

void Page::setLevel(int level)
{
    m_level = level;
}

void Page::writeData(std::string& line)
{
    m_data += line;
}

void Page::save()
{
    if (m_fakeName.empty()) return;

    if (m_saveFolder[m_saveFolder.length() - 1] != '/') m_saveFolder += '/';

    fstream outf(m_saveFolder + m_fakeName + ".html", ios::out);
    outf.write(m_data.data(), m_data.size());
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

int Page::getLevel()
{
    return m_level;
}

string Page::readData()
{
    return m_data;
}

string Page::getLink()
{
    return m_path + m_name;
}

bool Page::extractLinkFromData(std::string& str, std::string& href)
{
    if (str.empty()) return false;
    string hrefValStart = "href=\"";
    string hrefValEnd = "\"";
    if (href.empty())
    {
        size_t pos1 = str.find(hrefValStart, 0);
        if (pos1 != string::npos)
        {
            size_t copyPos1 = pos1 + hrefValStart.length();
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
            href += str;
            return false;
        }
        href += str.substr(0, pos1);
        Util::trim(href);
        str.erase(0, pos1 + hrefValEnd.length());
    }
    if (href.empty()) return false;
    return true;
}

deque<string> Page::findLinks()
{
    deque<string> links;
    string currentHref;

    std::istringstream iss(m_data);

    while (!iss.eof())
    {
        // search link
        string lineStr;
        iss >> lineStr;

        while (extractLinkFromData(lineStr, currentHref))
        {
            cout << "link:" << currentHref << endl;
            links.push_back(currentHref);
            currentHref.clear();
        }
    }
    return links;
}

void Page::replaceLink(string& link, string& fakeName)
{
    string whatLink = link;
    string pseudoName = fakeName + ".html";
    if (link == "/")
    {
        whatLink = "\"/\"";
        pseudoName = "\"" + fakeName + ".html\"";
    }
    Util::substitute(m_data, whatLink, pseudoName);
}
