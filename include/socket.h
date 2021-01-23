#ifndef BASESOCKET_H
#define BASESOCKET_H

#include <string>
#include "inputstream.h"
#include "outputstream.h"

class Socket : public InputStream, public OutputStream
{
    int m_sock;
    std::string m_remoteIP;
    int m_remotePort;
    bool m_connected;
    void open(std::string& host, int port);

public:
    Socket();
    Socket(std::string& host, int port);
    Socket(int socket);
    ~Socket();
    void setRemoteIP(std::string& remoteIP);
    std::string getRemoteIP();
    void setRemotePort(int remotePort);
    int getRemotePort();
    bool waitUnbuffered(int timeout);
    int readSomeUnbuffered(void* buf,int len);
    int writeSome(const void* buf,int len);
};

#endif // BASESOCKET_H
