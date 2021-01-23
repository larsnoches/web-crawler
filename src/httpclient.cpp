#include "httpclient.h"

#include <stdexcept>
#include <iostream>
#include "socket.h"
#include "httpheader.h"

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
        HttpHeader httpHeader;
        httpHeader.setHost(host);
//        httpHeader.setResource("/");
        socket.write(httpHeader.buildHeader());
        cout << httpHeader.buildHeader() << endl;
        while (!socket.isEof())
        {
            if (!socket.wait(5000)) break;
            st = socket.readLine();
            if (st.empty()) break;
            httpHeader.parseLine(st);
        }

    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
