#include "httpclient.h"
#include "socket.h"
#include "httprequestheader.h"
#include "httpresponseheader.h"

#include <stdexcept>
#include <iostream>

using namespace std;

HttpClient::HttpClient(std::string& url)
    : m_url(url)
{
    //
}

void HttpClient::run()
{
    try
    {
        string proto = "http://";
        int urlStartsAt = m_url.find(proto, 0);
        bool withProto = urlStartsAt >= 0;
        int urlEndsAt = withProto ? m_url.find("/", urlStartsAt + proto.length()) : m_url.find("/", 0);
        string host = withProto ? m_url.substr(proto.length(), urlEndsAt - proto.length()) : m_url.substr(0, urlEndsAt);

        int port = 80;
        Socket socket(host, port);
        string st;
        HttpRequestHeader httpRequestHeader;
        httpRequestHeader.setHost(host);
        httpRequestHeader.setResource("/");
        cout << httpRequestHeader.buildHeader() << endl;
        socket.write(httpRequestHeader.buildHeader());

        HttpResponseHeader httpResponseHeader;
        while (!socket.isEof())
        {
            if (!socket.wait(5000)) break;
            st = socket.readLine();
            if (st.empty()) break;
//            httpRequestHeader.parseLine(st);
            httpResponseHeader.parseLine(st);
        }

    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
