/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef BASESOCKET_H
#define BASESOCKET_H

#include <string>
#include <openssl/ssl.h>
#include "inputstream.h"
#include "outputstream.h"

class Socket : public InputStream, public OutputStream
{
    int m_sock;
    std::string m_remoteIP;
    int m_remotePort;
    bool m_secure;
    SSL_CTX* m_sslCtx;
    SSL* m_ssl;
    void open(std::string& host, int port);
    void initSslCtx();
    void makeSecureConnection();

public:
    Socket(std::string& host, int port, bool secure = false);
    ~Socket();
    void close();
    void setRemoteIP(std::string& remoteIP);
    std::string getRemoteIP();
    void setRemotePort(int remotePort);
    int getRemotePort();
    bool waitUnbuffered(int timeout);
    int readSomeUnbuffered(void* buf,int len);
    int writeSome(const void* buf,int len);
};

#endif // BASESOCKET_H
