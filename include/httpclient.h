#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "socket.h"

#include <string>

class HttpClient
{
    std::string m_host;
    std::string m_resource;
    int m_port;
    bool m_useSecure;
    bool m_useGzipEncoding;
    bool m_bodyChunked;
    int cropHost(std::string url);
    void cropResource(int pos, std::string url);
    bool gzipInflate(const std::string& compressedBytes, std::string& uncompressedBytes);
    std::string readBody(Socket& socket, int chunkLen = 0);

public:
    HttpClient(std::string& url, bool useGzipEncoding = false);
    void run();
};

#endif // HTTPCLIENT_H
