/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef HTTPRESPONSEHEADER_H
#define HTTPRESPONSEHEADER_H

#include <string>
#include <map>

// http result for response header
enum HttpResult
{
    HttpOK, // 200 OK
    HttpPartialContent, // 206 Partial Content
    HttpMovedPermanently, // 301 Moved Permanently
    HttpFound, // 302 Found
    HttpSeeOther, // 303 See Other
    HttpNotModified, // 304 Not Modified
    HttpBadRequest, // 400 Bad Request
    HttpForbidden, // 403 Forbidden
    HttpNotFound, // 404 Not Found
    HttpMethodNotAllowed, // 405 Method Not Allowed
    HttpInternalServerError, // 500 Internal Server Error
    HttpBadGateway // 502 Bad Gateway
};

// cookie in the response header
struct ResponseCookie
{
    // cookie's value
    std::string value;
    // cookie's expiration time
    time_t expireTime;
};

class HttpResponseHeader
{
private:
    // http result
    HttpResult m_result;
    // server header value
    std::string m_server;
    // the mime type of the content
    std::string m_contentType;
    // encoding of content
    std::string m_contentEncoding;
    // encoding of transfer
    std::string m_transferEncoding;
    // redirect location
    std::string m_location;
    // content-length header value
    long long m_contentLength;
    // content range
    long long m_rangeFrom, m_rangeTo, m_rangeTotal;
    // last-modified header value, should be 0 if not used
    time_t m_modifyTime;
    // expires header value, should be 0 if not used
    time_t m_expireTime;
    // cookies as name-value pairs, cookie's expiration time is additionally stored
    std::map<std::string, ResponseCookie> m_cookies;
    // allows to set some custom header, e.g. X-Accel-Redirect for nginx
    std::map<std::string, std::string> m_customHeaders;

    // parse cookies string
    void parseCookies(const std::string& st);
    // parse single header item string
    void parseHeaderItem(const std::string& name, const std::string& value);

public:
    // constructs a new header with default values
    HttpResponseHeader();
    // parse a single line of header data
    void parseLine(const std::string& line);
    HttpResult getResult();
    std::string getServer();
    std::string getContentType();
    std::string getContentEncoding();
    std::string getTransferEncoding();
    std::string getLocation();
    long long getContentLength();
    long long getRangeFrom();
    long long getRangeTo();
    long long getRangeTotal();
    time_t getModifyTime();
    time_t getExpireTime();
    std::map<std::string, ResponseCookie> getCookies();
    std::map<std::string, std::string> getCustomHeaders();

};

#endif // HTTPRESPONSEHEADER_H
