#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "socket.h"
#include "page.h"
#include "httpresponseheader.h"

#include <string>
#include <deque>
#include <map>

class HttpClient
{
    std::string m_host;
    std::string m_resource;
    std::string m_path;
    int m_port;
    bool m_useSecure;
    bool m_useGzipEncoding;
    bool m_bodyChunked;
    int m_pageCounter;
    int m_walkLevel;
    std::deque<Page> m_pages;
    // url, fakeName
    std::map<std::string, std::string> m_processedUrls;

    std::string cropHost(std::string url);
    std::string cropResource(std::string host, std::string url);
    void parseLinks(Page& page);
    Page isolatePage(std::string res, int prevLevel);
    bool addPage(Page& page);

    bool readHeader(Socket& socket,
                    Page& page,
                    HttpResponseHeader& httpResponseHeader);
    std::string gzipDecompress(const char* data, std::size_t size);
    std::string readBody(Socket& socket, int chunkLen = 0);
    void makeRequest(Socket& socket, Page& page);
    void getResponse(Socket& socket, Page& page);
    void run_d();

public:
    HttpClient(std::string& url,
               bool useGzipEncoding = false,
               int walkLevel = 3);
    void start();
};

#endif // HTTPCLIENT_H
