/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef HTTPREQUESTHEADER_H
#define HTTPREQUESTHEADER_H

#include <string>
#include <map>

enum HttpMethod
{
    HttpGet,
    HttpPost
};

class HttpRequestHeader
{
    // HTTP method, GET is the default one
    HttpMethod m_method;
    // host header value
    std::string m_host;
    // requested resource
    std::string m_resource;
    // if-modified-since header value, should be 0 if not used
    time_t m_modifyTime;
    // user-agent header value
    std::string m_userAgent;
    // content range, sould be -1 if not used
    long long m_rangeFrom;
    long long m_rangeTo;
    // cookies as name-value pairs
    std::map<std::string,std::string> m_cookies;
    // the mime type of the content
    std::string m_contentType;
    // content-length header value, should be 0 if not used
    int m_contentLength;
    // allows to set some custom header
    std::map<std::string,std::string> m_customHeaders;
    // is this connection persistent
    bool m_keepConnection;
    // what encoding client accepts (gzip)
    std::string m_acceptEncoding;

    // parse cookies string
    void parseCookies(const std::string& st);
    // parse single header item string
    void parseHeaderItem(const std::string& name,const std::string& value);

public:
    // constructs a new header with default values
    HttpRequestHeader();
    // parse a single line of header data
    void parseLine(const std::string& line);
    // writes the entire header into string
    std::string buildHeader();

    void setKeepConnetion(bool keepConnection);
    void setHost(std::string& host);
    void setResource(const char* resource);
    void setResource(std::string& resource);
    void setCookies(std::map<std::string,std::string>& cookies);
    void setContentType(std::string& contentType);
    void setContentLength(int contentLength);
    void setCustomHeaders(std::map<std::string,std::string>& customHeaders);
    void setAcceptEncoding(std::string acceptEncoding);
};

#endif // HTTPREQUESTHEADER_H
