#include "httpclient.h"
#include "socket.h"
#include "httprequestheader.h"
#include "httpresponseheader.h"

#include <stdexcept>
#include <iostream>

using namespace std;

HttpClient::HttpClient(std::string& url)
    : m_port(80),
      m_isSecure(false)
{
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

void HttpClient::run()
{
    try
    {
        Socket socket(m_host, m_port, m_isSecure);
        string st;
        HttpRequestHeader httpRequestHeader;
        httpRequestHeader.setHost(m_host);
        httpRequestHeader.setResource(m_resource);
        cout << httpRequestHeader.buildHeader() << endl;
        socket.write(httpRequestHeader.buildHeader());

        bool isHttpHeader = true;
        HttpResponseHeader httpResponseHeader;
        while (!socket.isEof())
        {
            if (!socket.wait(5000)) break;
            st = socket.readLine();
//            if (st.empty()) break;
//            httpRequestHeader.parseLine(st);
            cout << st << endl;

            if (st.empty())
            {
                isHttpHeader = false;
//                continue;
            }

            if (isHttpHeader)
            {
                httpResponseHeader.parseLine(st);
                HttpResult httpResult = httpResponseHeader.getResult();
                if (httpResult == HttpResult::HttpOK) continue;

                string contentType = httpResponseHeader.getContentType();
                if (!contentType.empty() && contentType.find("text/html", 0) == string::npos)
                {
                    throw runtime_error("This program is configured for html reading only.");
                }

                bool shouldRedirect = (httpResult == HttpResult::HttpMovedPermanently
                    || httpResult == HttpResult::HttpFound || httpResult == HttpResult::HttpSeeOther);

                string location = httpResponseHeader.getLocation();
                if (shouldRedirect && !location.empty())
                {
                    if (location.find("https://", 0) != string::npos)
                    {
                        m_isSecure = true;
                        m_port = 443;
                    }
                    int resourceStartPos = cropHost(location);
                    if (resourceStartPos > 0) cropResource(resourceStartPos, location);
                    run();
                    return;
                }
//                continue;
            }

//            cout << st << endl;
        }

    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
