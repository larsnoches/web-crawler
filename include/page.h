#ifndef PAGE_H
#define PAGE_H

#include <string>
//#include <vector>

class Page
{
    std::string m_name;
    std::string m_path;
    std::string m_fakeName;
//    std::vector<char> m_data;
    std::string m_data;

public:
//    Page(Page& p) {};
    Page();
    void setName(std::string& name);
    void setPath(std::string& path);
    void setFakeName(std::string& fakeName);
    void writeData(std::string& line);
    void save();

    std::string getName();
    std::string getPath();
    std::string getFakeName();
    std::string readData();
    std::string getLink();
    void replaceLink(std::string& link);
};

#endif // PAGE_H
