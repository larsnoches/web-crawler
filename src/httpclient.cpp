#include "httpclient.h"
#include "socket.h"
#include "httprequestheader.h"
#include "httpresponseheader.h"

#include <stdexcept>
#include <iostream>

using namespace std;

HttpClient::HttpClient(std::string& url)
    : m_port(80)
{
    cropHost(url);
}

void HttpClient::cropHost(string url)
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
    cropResource(fullStartPos, url);
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
        Socket socket(m_host, m_port);
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

//            cout << st << endl;

            if (st.empty())
            {
                isHttpHeader = false;
                continue;
            }

            if (isHttpHeader)
            {
                httpResponseHeader.parseLine(st);
            }
            else
            {
                cout << st << endl;
            }

        }

    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
