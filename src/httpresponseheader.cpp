#include "httpresponseheader.h"
#include "util.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

using namespace std;

const int resultCodes[]={
    200,
    206,
    301,
    302,
    303,
    304,
    400,
    403,
    404,
    405,
    500,
    502,
    0
};

const char* const resultStrings[]={
    "200 OK",
    "206 Partial Content",
    "301 Moved Permanently",
    "302 Found",
    "303 See Other",
    "304 Not Modified",
    "400 Bad Request",
    "403 Forbidden",
    "404 Not Found",
    "405 Method Not Allowed",
    "500 Internal Server Error",
    "502 Bad Gateway"
};

HttpResponseHeader::HttpResponseHeader()
    : m_result(HttpOK),
      m_server(""),
      m_contentLength(0),
      m_rangeFrom(-1),
      m_modifyTime(0),
      m_expireTime(0)
{
    //
}

void HttpResponseHeader::parseHeaderItem(const string& name,
                                         const string& value)
{
    // Some additional verifications applied to ensure nothing wrong happens later.
    if (name == "Server")
    {
        m_server = value;
        return;
    }
    if (name == "Content-Length")
    {
        m_contentLength = atoi(value.c_str());
        if (m_contentLength < 0) throw runtime_error("HttpBadRequest. Malformed header.");
        return;
    }
    if (name == "Content-Type")
    {
        m_contentType = value;
        return;
    }
    if (name == "Location")
    {
        m_location = value;
        return;
    }
    if (name == "Content-Range")
    {
        size_t sppos = value.find(' ');
        size_t mpos = value.find('-');
        size_t spos = value.find('/');
        if ((sppos == string::npos) || (mpos == string::npos) || (spos == string::npos))
        {
            throw runtime_error("HttpBadRequest. Malformed header.");
        }
        m_rangeFrom = atoll(value.substr(sppos + 1, mpos - sppos - 1).c_str());
        m_rangeTo = atoll(value.substr(mpos + 1, spos - mpos - 1).c_str());
        m_rangeTotal = atoll(value.substr(spos + 1).c_str());
        if ((m_rangeFrom < 0) || (m_rangeTo < 0) || (m_rangeTotal < 0) || (m_rangeTo < m_rangeFrom)
            || (m_rangeFrom > m_rangeTotal) || (m_rangeTo > m_rangeTotal))
        {
            throw runtime_error("HttpBadRequest. Malformed header.");
        }
        return;
    }
    if (name == "Last-Modified")
    {
        m_modifyTime = Util::parseHttpTime(value);
        return;
    }
    if (name == "Expires")
    {
        m_expireTime = Util::parseHttpTime(value);
        return;
    }
    if (name == "Set-Cookie")
    {
        parseCookies(value);
        return;
    }
    m_customHeaders[name] = value;
}

void HttpResponseHeader::parseLine(const string& line)
{
    size_t colonPos = line.find(':');
    if (colonPos == string::npos)
    {
        vector<string> items = Util::extract(line);
        int code = atoi(items[1].c_str());
        for (int i = 0; resultCodes[i] != 0; i++)
        {
            if (resultCodes[i] == code)
            {
                m_result = static_cast<HttpResult>(i);
                break;
            }
        }
    }
    else
    {
        string name = line.substr(0, colonPos);
        if (line.length() < colonPos + 3) throw runtime_error("HttpBadRequest. Unexpected end of line in the header");
        string value = line.substr(colonPos + 2);
        parseHeaderItem(name, value);
    }
}

void HttpResponseHeader::parseCookies(const string& st)
{
    size_t eqpos = st.find("name=");
    eqpos += 4;
    size_t scpos = st.find(';', eqpos);
    size_t eq2pos = st.find("expires=");
    eq2pos += 7;
    size_t sc2pos = st.find(';', eq2pos);
    string name = st.substr(0, eqpos);
    m_cookies[name].value = st.substr(eqpos + 1, scpos - eqpos - 1);
    if (eq2pos != string::npos)
    {
        m_cookies[name].expireTime = Util::parseHttpTime(st.substr(eq2pos + 1, sc2pos - eq2pos - 1));
    }
    else
    {
        m_cookies[name].expireTime = 0;
    }
}

HttpResult HttpResponseHeader::getResult()
{
    return m_result;
}

string HttpResponseHeader::getServer()
{
    return m_server;
}

string HttpResponseHeader::getContentType()
{
    return m_contentType;
}

string HttpResponseHeader::getLocation()
{
    return m_location;
}

long long HttpResponseHeader::getContentLength()
{
    return m_contentLength;
}

long long HttpResponseHeader::getRangeFrom()
{
    return m_rangeFrom;
}

long long HttpResponseHeader::getRangeTo()
{
    return m_rangeTo;
}

long long HttpResponseHeader::getRangeTotal()
{
    return m_rangeTotal;
}

time_t HttpResponseHeader::getModifyTime()
{
    return m_modifyTime;
}

time_t HttpResponseHeader::getExpireTime()
{
    return m_expireTime;
}

map<string, ResponseCookie> HttpResponseHeader::getCookies()
{
    return m_cookies;
}

map<string, string> HttpResponseHeader::getCustomHeaders()
{
    return m_customHeaders;
}
