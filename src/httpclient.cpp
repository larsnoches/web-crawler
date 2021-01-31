#include "httpclient.h"
#include "httprequestheader.h"
#include "httpresponseheader.h"
#include "util.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <ios>
#include <zlib.h>

using namespace std;

HttpClient::HttpClient(std::string& url, bool useGzipEncoding)
    : m_port(80),
      m_useSecure(false),
      m_useGzipEncoding(useGzipEncoding),
      m_bodyChunked(false)
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

bool HttpClient::gzipInflate(const string& compressedBytes, string& uncompressedBytes)
{
    if (compressedBytes.size() == 0)
    {
        uncompressedBytes = compressedBytes;
        return true;
    }

    uncompressedBytes.clear();

    unsigned full_length = compressedBytes.size();
    unsigned half_length = compressedBytes.size() / 2;

    unsigned uncompLength = full_length;
    char* uncomp = (char*)calloc(sizeof(char), uncompLength);

    z_stream strm;
    strm.next_in = (Bytef*)compressedBytes.c_str();
    strm.avail_in = compressedBytes.size();
    strm.total_out = 0;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;

    bool done = false;

    if (inflateInit2(&strm, (16 + MAX_WBITS)) != Z_OK)
    {
        free(uncomp);
        return false;
    }

    while (!done)
    {
        // If our output buffer is too small
        if (strm.total_out >= uncompLength)
        {
            // Increase size of output buffer
            char* uncomp2 = (char*)calloc(sizeof(char), uncompLength + half_length);
            memcpy(uncomp2, uncomp, uncompLength);
            uncompLength += half_length;
            free(uncomp);
            uncomp = uncomp2;
        }

        strm.next_out = (Bytef*)(uncomp + strm.total_out);
        strm.avail_out = uncompLength - strm.total_out;

        // Inflate another chunk.
        int err = inflate(&strm, Z_SYNC_FLUSH);
        if (err == Z_STREAM_END)
            done = true;
        else if (err != Z_OK)
        {
            break;
        }
    }

    if (inflateEnd(&strm) != Z_OK)
    {
        free(uncomp);
        return false;
    }

    for (size_t i = 0; i < strm.total_out; ++i)
    {
        uncompressedBytes += uncomp[i];
    }
    free(uncomp);
    return true;
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
        if (m_useGzipEncoding)
        {
            string uncompressed;
            gzipInflate(data, uncompressed);
            data = uncompressed;
        }
        return data;
    }

    // handling ordinary data
    string ordinaryLine = socket.readLine();
    if (m_useGzipEncoding)
    {
        string uncompressed;
        gzipInflate(ordinaryLine, uncompressed);
        ordinaryLine = uncompressed;
    }
    return ordinaryLine;
}

void HttpClient::run()
{
    try
    {
        Socket socket(m_host, m_port, m_useSecure);

        HttpRequestHeader httpRequestHeader;
        httpRequestHeader.setHost(m_host);
        httpRequestHeader.setResource(m_resource);
        if (m_useGzipEncoding) httpRequestHeader.setAcceptEncoding("gzip");

        map<string, string> customHeaders = {
            {
                "Accept",
                "text/html,application/xhtml+xml,"
                "application/xml;q=0.9,image/avif,"
                "image/webp,image/apng,*/*;q=0.8,"
                "application/signed-exchange;v=b3;q=0.9"
            },
            {
                "Accept-Language",
                "ru-RU,ru;q=0.9"
            },
        };
        httpRequestHeader.setCustomHeaders(customHeaders);

        cout << httpRequestHeader.buildHeader() << endl;
        socket.write(httpRequestHeader.buildHeader());

        bool isHttpHeader = true;
        bool shouldRedirect = false;
        HttpResponseHeader httpResponseHeader;
        fstream outf("page.htm", ios::out);
        while (!socket.isEof())
        {
            if (!socket.wait(5000)) break;

            // body
            if (!isHttpHeader)
            {
                string data = readBody(socket);
                cout << data << endl;
                outf << data;
                continue;
            }

            // header
            string st = socket.readLine();
            if (st.empty() && isHttpHeader)
            {
                isHttpHeader = false;
                continue;
            }
            cout << st << endl;

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
                    run();
                    return;
                }
            }

            // check for chunked data
            if (!m_bodyChunked)
            {
                string transferEncoding = httpResponseHeader.getTransferEncoding();
                if (!transferEncoding.empty() && transferEncoding.find("chunked") != string::npos)
                {
                    m_bodyChunked = true;
                }
            }

            // ordinary data
            if (httpResult == HttpResult::HttpOK) continue;

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
                run();
                return;
            }
        }
        outf.close();
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
