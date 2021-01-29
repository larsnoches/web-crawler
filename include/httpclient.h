#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

class HttpClient
{
    std::string m_host;
    std::string m_resource;
    int m_port;
    bool m_isSecure;
    int cropHost(std::string url);
    void cropResource(int pos, std::string url);

public:
    HttpClient(std::string& url);
    void run();
};

#endif // HTTPCLIENT_H
