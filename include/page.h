/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef PAGE_H
#define PAGE_H

#include <string>
#include <deque>

class Page
{
    std::string m_name;
    std::string m_path;
    std::string m_fakeName;
    std::string m_data;
    std::string m_saveFolder;
    int m_level;

    bool extractLinkFromData(std::string& str, std::string& href);

public:
    Page(std::string& saveFolder, int level = 1);
    void setName(std::string& name);
    void setPath(std::string& path);
    void setFakeName(std::string& fakeName);
    void setLevel(int level);
    void writeData(std::string& line);
    void save();

    std::string getName();
    std::string getPath();
    std::string getFakeName();
    int getLevel();
    std::string readData();
    std::string getLink();
    std::deque<std::string> findLinks();
    void replaceLink(std::string& link, std::string& fakeName);

};

#endif // PAGE_H
