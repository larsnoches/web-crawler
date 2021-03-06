/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#include "httprequestheader.h"
#include "util.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

using namespace std;

const char* const methodStrings[] = { "GET", "POST" };
const char* clientAgentString = "Mozilla/5.0 (Windows NT 6.3; WOW64) "
                                "AppleWebKit/537.36 (KHTML, like Gecko) "
                                "Chrome/87.0.4280.141 "
                                "Safari/537.36 OPR/73.0.3856.344";


HttpRequestHeader::HttpRequestHeader()
    : m_method(HttpGet),
      m_resource("/"),
      m_modifyTime(0),
      m_userAgent(clientAgentString),
      m_rangeFrom(-1),
      m_rangeTo(-1),
      m_contentLength(0),
      m_keepConnection(false)
{
    //
}

void HttpRequestHeader::parseHeaderItem(const string& name, const string& value)
{

    // some additional verifications applied to ensure nothing wrong happens later
    if (name == "Host")
    {
        m_host = value;
        return;
    }
    if (name == "User-Agent")
    {
        m_userAgent = value;
        return;
    }
    if (name == "Range")
    {
        size_t eqpos = value.find('=');
        size_t mpos = value.find('-');

        if ((eqpos == string::npos) || (mpos == string::npos)) throw runtime_error("Malformed header.");

        m_rangeFrom = atoll(value.substr(eqpos + 1, mpos - eqpos - 1).c_str());
        string t = value.substr(mpos + 1);
        if (!t.empty())
        {
            m_rangeTo = atoll(t.c_str());
            if (m_rangeTo < m_rangeFrom) throw runtime_error("Malformed header.");
        }
        return;
    }
    if (name == "Content-Length")
    {
        m_contentLength = atoi(value.c_str());
        if (m_contentLength < 0) throw runtime_error("Malformed header.");
        return;
    }
    if (name == "Cookie")
    {
        parseCookies(value);
        return;
    }
    if (name == "If-Modified-Since")
    {
        m_modifyTime = Util::parseHttpTime(value);
        return;
    }
    if (name == "Content-Type")
    {
        m_contentType = value;
        return;
    }
    if (name == "Connection")
    {
        string t = Util::stringToLower(value);
        if (t.find("keep-alive") != string::npos) m_keepConnection = true;
        if (t.find("close") != string::npos) m_keepConnection = false;
        return;
    }
    m_customHeaders[name] = value;
}

void HttpRequestHeader::parseLine(const string& line)
{
    size_t colonPos = line.find(':');
    if (colonPos == string::npos)
    {
        vector<string> items = Util::extract(line);
        if (items[0] == methodStrings[HttpPost])
        {
            m_method = HttpPost;
        }
        else
        {
            if (items[0] != methodStrings[HttpGet]) throw runtime_error("Only POST and GET methods are supported");
        }
        // parameters from resource will be extracted later
        // they are not related to the header itself
        m_resource = items[1];
        if (items[2] != "HTTP/1.0") m_keepConnection = true;
    }
    else
    {
        string name = line.substr(0, colonPos);
        if (line.length() < colonPos + 3) throw runtime_error("Unexpected end of line in the header");
        string value = line.substr(colonPos + 2);
        parseHeaderItem(name, value);
    }
}

std::string HttpRequestHeader::buildHeader()
{
    ostringstream r;
    r << methodStrings[m_method] << " " << m_resource << " "
      << "HTTP/1.1" << endl;
    r << "Host: " << m_host << endl;
    r << "User-Agent: " << m_userAgent << endl;

    if (!m_contentType.empty()) r << "Content-Type: " << m_contentType << endl;
    if (m_contentLength != 0) r << "Content-Length: " << m_contentLength << endl;

    if (m_rangeFrom != -1)
    {
        r << "Range: bytes=" << m_rangeFrom << "-";
        if (m_rangeTo != -1) r << m_rangeTo;
        r << endl;
    }
    if (m_modifyTime != 0) r << "If-Modified-Since: " << Util::makeHttpTime(m_modifyTime) << endl;
    if (!m_cookies.empty())
    {
        r << "Cookie: ";
        for (map<string, string>::iterator iter = m_cookies.begin(); iter != m_cookies.end(); iter++)
        {
            if (iter != m_cookies.begin()) r << "; ";
            r << iter->first << "=" << iter->second;
        }
        r << endl;
    }
    for (map<string, string>::iterator iter = m_customHeaders.begin();
         iter != m_customHeaders.end();
         iter++)
    {
        r << iter->first << ": " << iter->second << endl;
    }

    if (m_keepConnection)
    {
        r << "Connection: keep-alive" << endl;
    }
    else
    {
        r << "Connection: close" << endl;
    }

    if (!m_acceptEncoding.empty()) r << "Accept-Encoding: " << m_acceptEncoding << endl;

    r << endl;
    return r.str();
}

void HttpRequestHeader::parseCookies(const string& st)
{
    string t = st;
    string p, l, r;
    size_t sepPos, eqPos;
    while (!t.empty())
    {
        sepPos = t.find(';');
        if (sepPos != string::npos)
        {
            p = t.substr(0, sepPos);
            t.erase(0, sepPos + 1);
        }
        else
        {
            p = t;
            t.clear();
        }
        Util::trim(p);
        eqPos = p.find('=');

        if (eqPos == string::npos) throw runtime_error("Malformed cookie in HTTP request");

        l = Util::stringToLower(p.substr(0, eqPos));
        r = Util::stringToLower(p.substr(eqPos + 1));
        m_cookies[l] = r;
    }
}

void HttpRequestHeader::setKeepConnetion(bool keepConnection)
{
    m_keepConnection = keepConnection;
}

void HttpRequestHeader::setHost(string& host)
{
    m_host = host;
}

void HttpRequestHeader::setResource(const char* resource)
{
    m_resource = resource;
}

void HttpRequestHeader::setResource(string& resource)
{
    m_resource = resource;
}

void HttpRequestHeader::setCookies(map<string,string>& cookies)
{
    m_cookies = cookies;
}

void HttpRequestHeader::setContentType(string& contentType)
{
    m_contentType = contentType;
}

void HttpRequestHeader::setContentLength(int contentLength)
{
    m_contentLength = contentLength;
}

void HttpRequestHeader::setCustomHeaders(map<string,string>& customHeaders)
{
    m_customHeaders = customHeaders;
}

void HttpRequestHeader::setAcceptEncoding(string acceptEncoding)
{
    m_acceptEncoding = acceptEncoding;
}
