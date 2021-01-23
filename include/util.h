#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

class Util
{
public:
    static std::string htmlEscape(const std::string& st);
    static unsigned char hexConv(char t);
    static std::string stringToLower(const std::string& st);
    static std::string urlDecode(const std::string& st);
    static std::string urlEncode(const std::string& st);
    static std::vector<std::string> extract(const std::string& st);
    static void substitute(std::string& st, const std::string& what, const std::string& to);
    static std::string makeHttpTime(time_t t);
    static time_t parseHttpTime(const std::string& httpTime);
    static std::string utf8Encode(const std::wstring& st);
    static std::wstring utf8Decode(const std::string& st);
    static void trim(std::string& st);
    static std::string generateRandomString(int len);
    static std::string timestamp(time_t t);
    static std::string mimeType(const std::string& ext);
};

#endif // UTIL_H
