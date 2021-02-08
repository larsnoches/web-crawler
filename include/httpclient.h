#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "socket.h"
#include "page.h"

#include <string>
#include <deque>

class HttpClient
{
    std::string m_host;
    std::string m_resource;
    int m_port;
    bool m_useSecure;
    bool m_useGzipEncoding;
    bool m_bodyChunked;
    int m_pageCounter;
    std::deque<Page> m_pages;

    int cropHost(std::string url);
    void cropResource(int pos, std::string url);
    void parseLinks(Page& page);
    Page isolatePage();
    bool addPage(Page& page);

    bool readHeader(Socket& socket,
                    Page& page,
                    bool shouldRedirect = false);
    std::string gzipDecompress(const char* data, std::size_t size);
    std::string readBody(Socket& socket, int chunkLen = 0);
    void makeRequest(Socket& socket, Page& page);
    void getResponse(Socket& socket, Page& page);
    void run_d();

public:
    HttpClient(std::string& url, bool useGzipEncoding = false);
    void start();
};

#endif // HTTPCLIENT_H
