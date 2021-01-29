#ifndef HTTPRESPONSEHEADER_H
#define HTTPRESPONSEHEADER_H

#include <string>
#include <map>

// Http result for response header.
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

// Cookie in the response header.
struct ResponseCookie
{
    // Cookie's value
    std::string value;
    // Cookie's expiration time.
    time_t expireTime;
};

class HttpResponseHeader
{
private:
    // Http result. See HttpResult enum.
    HttpResult m_result;
    // Server header value.
    std::string m_server;
    // The mime type of the content.
    std::string m_contentType;
    // Redirect location.
    std::string m_location;
    // Content-Length header value.
    long long m_contentLength;
    // Content range. Used with HttpPartialContent result. Should be -1 if not used.
    long long m_rangeFrom, m_rangeTo, m_rangeTotal;
    // Last-Modified header value. Should be 0 if not used.
    time_t m_modifyTime;
    // Expires header value. Can be used to avoid subsequent requests to static content. Should be
    // 0 if not used.
    time_t m_expireTime;
    // Cookies as name-value pairs. Cookie's expiration time is additionally stored.
    std::map<std::string, ResponseCookie> m_cookies;
    // Allows to set some custom header, e.g. X-Accel-Redirect for nginx.
    std::map<std::string, std::string> m_customHeaders;

    // Parse cookies string.
    void parseCookies(const std::string& st);
    // Parse single header item string.
    void parseHeaderItem(const std::string& name, const std::string& value);

public:
    // Constructs a new header with default values.
    HttpResponseHeader();
    // Parse a single line of header data.
    void parseLine(const std::string& line);
    HttpResult getResult();
    std::string getServer();
    std::string getContentType();
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
