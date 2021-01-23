#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

class HttpClient
{
    std::string m_url;
public:
    HttpClient(std::string& url);
    void run();
};

#endif // HTTPCLIENT_H
