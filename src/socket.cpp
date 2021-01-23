#include "socket.h"

#include <memory.h>
#include <iostream>
#include "ioexception.h"

#ifdef WIN32
#include <Ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#endif

using namespace std;

#ifdef WIN32
struct WSInit
{
public:
    WSInit()
    {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
    }

    ~WSInit()
    {
        WSACleanup();
    }
} wsinit;
#endif

Socket::Socket()
    : m_sock(-1)
{
}

Socket::Socket(string& host, int port)
    : m_sock(-1)
{
    open(host, port);
}

Socket::Socket(int sock)
{
    this->m_sock = sock;
}

Socket::~Socket()
{
    if (m_sock >= 0)
    {
#ifdef WIN32
        closesocket(m_sock);
#else
        close(m_sock);
#endif
    }
}

void Socket::setRemoteIP(std::string& remoteIP)
{
    m_remoteIP = remoteIP;
}

std::string Socket::getRemoteIP()
{
    return m_remoteIP;
}

void Socket::setRemotePort(int remotePort)
{
    m_remotePort = remotePort;
}

int Socket::getRemotePort()
{
    return m_remotePort;
}

void Socket::open(string& host, int port)
{
    int sd, err;
    struct addrinfo hints = {}, *addrs;
//    char portStr[16] = {};

    hints.ai_family = AF_INET; // Since your original code was using sockaddr_in and
                               // PF_INET, I'm using AF_INET here to match.  Use
                               // AF_UNSPEC instead if you want to allow getaddrinfo()
                               // to find both IPv4 and IPv6 addresses for the hostname.
                               // Just make sure the rest of your code is equally family-
                               // agnostic when dealing with the IP addresses associated
                               // with this connection. For instance, make sure any uses
                               // of sockaddr_in are changed to sockaddr_storage,
                               // and pay attention to its ss_family field, etc...
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

//    sprintf(portStr, "%d", port);
    string portStr = to_string(port);

    err = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &addrs);
//    err = getaddrinfo(host.c_str(), portStr, &hints, &addrs);
    if (err != 0)
    {
//        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(err));
        cout << gai_strerror(err) << endl;
        abort();
    }

    for(struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sd == -1)
        {
            err = errno;
            break; // if using AF_UNSPEC above instead of AF_INET/6 specifically,
                   // replace this 'break' with 'continue' instead, as the 'ai_family'
                   // may be different on the next iteration...
        }

        if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0)
            break;

        err = errno;

#ifdef WIN32
        closesocket(sd);
#else
        close(sd);
#endif
        sd = -1;
    }

    freeaddrinfo(addrs);
    if (sd == -1)
    {
        cout << strerror(err) << endl;
        abort();
    }
    m_sock = sd;
}

bool Socket::waitUnbuffered(int timeout)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_sock, &readfds);
    timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    select(m_sock + 1, &readfds, NULL, NULL, &tv);
    return FD_ISSET(m_sock, &readfds) != 0;
}

int Socket::readSomeUnbuffered(void* buf, int len)
{
    int br = recv(m_sock, reinterpret_cast<char*>(buf), len, 0);
    if (br < 0) throw IOException("Socket read error");
    return br;
}

int Socket::writeSome(const void* buf, int len)
{
    int bw = send(m_sock, reinterpret_cast<const char*>(buf), len, 0);
    if (bw < 0) throw IOException("Socket write error");
    return bw;
}
