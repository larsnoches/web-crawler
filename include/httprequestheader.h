#ifndef HTTPREQUESTHEADER_H
#define HTTPREQUESTHEADER_H

#include <string>
#include <map>

// Http method. Only GET and POST are usable.
enum HttpMethod
{
    HttpGet,
    HttpPost
};

class HttpRequestHeader
{
    // HTTP method, GET is the default one. See HttpMethod enum.
    HttpMethod m_method;
    // Host header value.
    std::string m_host;
    // Requested resource.
    std::string m_resource;
    // If-Modified-Since header value. Should be 0 if not used.
    time_t m_modifyTime;
    // User-Agent header value.
    std::string m_userAgent;
    // Content range. Should be -1 if not used.
    long long m_rangeFrom;
    long long m_rangeTo;
    // Cookies as name-value pairs.
    std::map<std::string,std::string> m_cookies;
    // The mime type of the content.
    std::string m_contentType;
    // Content-Length header value. Should be 0 if not used.
    int m_contentLength;
    // Allows to set some custom header.
    std::map<std::string,std::string> m_customHeaders;
    // Is this connection persistent
    bool m_keepConnection;

    // Parse cookies string.
    void parseCookies(const std::string& st);
    // Parse single header item string.
    void parseHeaderItem(const std::string& name,const std::string& value);

public:
    // Constructs a new header with default values.
    HttpRequestHeader();
    // Parse a single line of header data.
    void parseLine(const std::string& line);
    // Writes the entire header into string.
    std::string buildHeader();

    void setHost(std::string& host);
    void setResource(const char* resource);
    void setResource(std::string& resource);
    void setCookies(std::map<std::string,std::string>& cookies);
    void setContentType(std::string& contentType);
    void setContentLength(int contentLength);
    void setCustomHeaders(std::map<std::string,std::string>& customHeaders);
};

#endif // HTTPREQUESTHEADER_H
