#include "httpclient.h"
#include "httprequestheader.h"
#include "httpresponseheader.h"
#include "util.h"
#include "gzipdecompressor.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>

using namespace std;

HttpClient::HttpClient(std::string& url, bool useGzipEncoding)
    : m_port(80),
      m_useSecure(false),
      m_useGzipEncoding(useGzipEncoding),
      m_bodyChunked(false),
      m_pageCounter(0)
{
    if (url.find("https://", 0) != string::npos)
    {
        m_port = 443;
        m_useSecure = true;
    }
    int resourceStartPos = cropHost(url);
    if (resourceStartPos > 0) cropResource(resourceStartPos, url);
}

int HttpClient::cropHost(string url)
{
    string proto = "://";
    // Find protocol in url
    int hostEndsAt = 0;
    int hostStartsAt = url.find(proto, 0);
    int fullStartPos = 0;

    if (hostStartsAt != (int)string::npos)
    {
        // Set full position for crop statring
        fullStartPos = hostStartsAt + proto.length();
    }

    // Find first slash at url
    hostEndsAt = url.find("/", fullStartPos);
    // Crop the host
    if (hostEndsAt != (int)string::npos)
    {
        m_host = url.substr(fullStartPos, hostEndsAt - fullStartPos);
    }
    else
    {
        // host would be a whole url string if fullStartPos is 0
        m_host = url.substr(fullStartPos, url.length() - fullStartPos);
    }
    return fullStartPos;
}

void HttpClient::cropResource(int pos, string url)
{
    // Crop the resource
    int resourceStartPos = m_host.length() + pos;
    if (url.length() - resourceStartPos > 0)
    {
        m_resource = url.substr(resourceStartPos, url.length() - resourceStartPos);
    }
    else
    {
        m_resource = "/";
    }
}

void HttpClient::parseLinks(Page& page)
{
    deque<string> links = page.findLinks();
}

/*
/index?p=what
/index.html
/pages/index
*/

Page HttpClient::isolatePage()
{
    string res = m_resource;
    Util::substitute(res, "...", "");

    // find slash
    size_t nextSlashPos = 0, slashPos = 0;
    while (nextSlashPos != string::npos)
    {
        nextSlashPos = res.find("/", slashPos + 1);
        if (nextSlashPos != string::npos) slashPos = nextSlashPos;
    }
    // copy str from next symbol after slash
    string pageName = res.substr(slashPos + 1, res.length() - slashPos);
    string path = slashPos > 0 ? res.substr(0, slashPos + 1) : "";

    size_t paramsPos = pageName.find("?");
    if (paramsPos != string::npos)
    {
        pageName = pageName.substr(0, paramsPos);
//        size_t dotPos = pageName.find(".");
//        if (dotPos != string::npos)
//        {
//            pageName = pageName.substr(0, dotPos);
//        }
//        return pageName;
    }

    size_t diezPos = pageName.find("#");
    if (diezPos != string::npos)
    {
        pageName = pageName.substr(0, diezPos);
//        pageName = res.substr(slashPos, diezPos - slashPos);
//        size_t dotPos = pageName.find(".");
//        if (dotPos != string::npos)
//        {
//            pageName = pageName.substr(0, dotPos);
//        }
//        return pageName;
    }

//    pageName = res.substr(slashPos, res.length() - slashPos);
    size_t dotPos = pageName.find(".");
    if (dotPos != string::npos)
    {
        pageName = pageName.substr(0, dotPos);
    }

    Page page;
    page.setName(pageName);
    page.setPath(path);

    string fakeName = "page" + to_string(++m_pageCounter);
    page.setFakeName(fakeName);

    return page;
}

bool HttpClient::addPage(Page& page)
{
    string pageLink = page.getLink();
    for (Page item : m_pages)
    {
        string itemLink = item.getLink();
        if (itemLink == pageLink)
        {
            return false;
        }
    }
    m_pages.push_back(page);
    return true;
}

bool HttpClient::readHeader(Socket& socket,
                            Page& page,
                            bool shouldRedirect)
{
    string st = socket.readLine();
    if (st.empty()) return false;

    cout << st << endl;

    HttpResponseHeader httpResponseHeader;
    httpResponseHeader.parseLine(st);
    HttpResult httpResult = httpResponseHeader.getResult();

    // only text/html saving
    string contentType = httpResponseHeader.getContentType();
    if (!contentType.empty() && contentType.find("text/html", 0) == string::npos)
    {
        throw runtime_error("This program is configured for html reading only.");
    }

    // reconnect with gzip encoding
    if (!m_useGzipEncoding)
    {
        string contentEncoding = httpResponseHeader.getContentEncoding();
        if (!contentEncoding.empty() && contentEncoding.find("gzip", 0) != string::npos)
        {
            m_useGzipEncoding = true;

            // another request
            socket.close();
            Socket anotherSocket(m_host, m_port, m_useSecure);
            makeRequest(anotherSocket, page);
            getResponse(anotherSocket, page);
            return true;
        }
    }

    // check for chunked data
    if (!m_bodyChunked)
    {
        string transferEncoding = httpResponseHeader.getTransferEncoding();
        if (!transferEncoding.empty() &&
            transferEncoding.find("chunked") != string::npos)
        {
            m_bodyChunked = true;
        }
    }

    // ordinary data
    if (httpResult == HttpResult::HttpOK)
    {
        return readHeader(socket, page);
    }

    // redirect to specified location
    shouldRedirect = (httpResult == HttpResult::HttpMovedPermanently
        || httpResult == HttpResult::HttpFound || httpResult == HttpResult::HttpSeeOther);

    string location = httpResponseHeader.getLocation();
    if (shouldRedirect && !location.empty())
    {
        if (location.find("https://", 0) != string::npos)
        {
            m_useSecure = true;
            m_port = 443;
        }
        int resourceStartPos = cropHost(location);
        if (resourceStartPos > 0) cropResource(resourceStartPos, location);

        // another request
        socket.close();
        Socket anotherSocket(m_host, m_port, m_useSecure);
        makeRequest(anotherSocket, page);
        getResponse(anotherSocket, page);
        return true;
    }
    return readHeader(socket, page, shouldRedirect);
}

string HttpClient::gzipDecompress(const char* data, std::size_t size)
{
    GzipDecompressor decomp;
    string output;
    decomp.decompress(output, data, size);
    return output;
}

string HttpClient::readBody(Socket& socket, int chunkLen)
{
    // handling chunked data
    if (m_bodyChunked && chunkLen == 0)
    {
        string st = socket.readLine();
        if (st.empty()) return "";
        chunkLen = stoi(st, nullptr, 16);
        return readBody(socket, chunkLen);
    }
    if (m_bodyChunked && chunkLen > 0)
    {
        if (socket.isEof() || !socket.wait(5000)) return "";
        string data = socket.read(chunkLen);
        return data;
    }

    // handling ordinary data
    string ordinaryLine = socket.readLine();
    return ordinaryLine;
}

void HttpClient::makeRequest(Socket& socket, Page& page)
{
    HttpRequestHeader httpRequestHeader;
    httpRequestHeader.setHost(m_host);

    m_resource = page.getPath();
    httpRequestHeader.setResource(m_resource);

//    Page page = isolatePage();
//    if (!addPage(page)) return;

    if (m_useGzipEncoding) httpRequestHeader.setAcceptEncoding("gzip");

    map<string, string> customHeaders = {
        {
            "Accept",
            "text/html"
        },
        {
            "Accept-Language",
            "ru-RU,ru;q=0.9"
        }
    };
    httpRequestHeader.setCustomHeaders(customHeaders);

    cout << httpRequestHeader.buildHeader() << endl;
    socket.write(httpRequestHeader.buildHeader());
}

void HttpClient::getResponse(Socket& socket, Page& page)
{
    bool isHttpHeader = true;
//    bool shouldRedirect = false;
//    HttpResponseHeader httpResponseHeader;
    ostringstream pageGzipStream;

    while (!socket.isEof())
    {
        if (!socket.wait(5000)) break;

        // body
        if (!isHttpHeader)
        {
            string data = readBody(socket);
            if (m_useGzipEncoding)
            {
                pageGzipStream << data;
                continue;
            }
            page.writeData(data);
            continue;
        }

        // header
        isHttpHeader = readHeader(socket, page);
    }

    if (m_useGzipEncoding)
    {
        string pageCompressed = pageGzipStream.str();
        string pageDecompressed = gzipDecompress(pageCompressed.data(), pageCompressed.size());
        page.writeData(pageDecompressed);
    }

    parseLinks(page);
    page.save();
//    m_pages.push_back(page);
}

void HttpClient::start()
{
    cout << "Starting" << endl;
    cout << "host: " << m_host << endl;
    cout << "path: " << m_resource << endl;
    cout << "proto: " << (m_useSecure ? "https" : "http") << endl;
    cout << "" << endl;

    Page indexPage = isolatePage();
    addPage(indexPage);
    run_d();

    cout << "" << endl;
    cout << "Stopping" << endl;
}

void HttpClient::run_d()
{
    while (!m_pages.empty())
    {
        Socket socket(m_host, m_port, m_useSecure);

        // get page from front
        Page pageItem = m_pages.front();
        m_pages.pop_front();

        makeRequest(socket, pageItem);
        getResponse(socket, pageItem);
    }
}

