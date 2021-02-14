/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#include "socket.h"

#include <memory.h>
#include <iostream>
#include "ioexception.h"

#ifdef WIN32
#include <Ws2tcpip.h>
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#endif
#include <openssl/err.h>

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

Socket::Socket(string& host, int port, bool secure)
    : m_sock(-1),
      m_secure(secure),
      m_sslCtx(nullptr),
      m_ssl(nullptr)
{
    if (m_secure) initSslCtx();
    open(host, port);
    if (m_secure) makeSecureConnection();
}

Socket::~Socket()
{
    close();
}

void Socket::close()
{
    try
    {
        if (m_secure)
        {
            SSL_shutdown(m_ssl);
            SSL_free(m_ssl);
        }
        if (m_sock >= 0)
        {
    #ifdef WIN32
            closesocket(m_sock);
    #else
            close(m_sock);
    #endif
        }
        if (m_secure) SSL_CTX_free(m_sslCtx);
    }
    catch (exception e) {
        //
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

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    string portStr = to_string(port);

    err = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &addrs);
    if (err != 0)
    {
        cout << gai_strerror(err) << endl;
        abort();
    }

    for(struct addrinfo* addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sd == -1)
        {
            err = errno;
            break;
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

void Socket::initSslCtx()
{
    // create new client-method instance
    const SSL_METHOD* method = TLS_client_method();
    m_sslCtx = SSL_CTX_new(method);

    if (m_sslCtx == nullptr)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    m_ssl = SSL_new(m_sslCtx);
    if (m_ssl == nullptr)
    {
        fprintf(stderr, "SSL_new() failed\n");
        exit(EXIT_FAILURE);
    }
}

void Socket::makeSecureConnection()
{
    if (!m_secure) return;
    SSL_set_fd(m_ssl, m_sock);
    const int status = SSL_connect(m_ssl);
    if (status != 1)
    {
        SSL_get_error(m_ssl, status);
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "SSL_connect failed with SSL_get_error code %d\n", status);
        exit(EXIT_FAILURE);
    }
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
    int br = 0;
    if (m_secure)
    {
        br = SSL_read(m_ssl, reinterpret_cast<char*>(buf), len);
    }
    else
    {
        br = recv(m_sock, reinterpret_cast<char*>(buf), len, 0);
    }

    if (br < 0) throw IOException("Socket read error");
    return br;
}

int Socket::writeSome(const void* buf, int len)
{
    int bw = 0;
    if (m_secure)
    {
        bw = SSL_write(m_ssl, reinterpret_cast<const char*>(buf), len);
    }
    else
    {
        bw = send(m_sock, reinterpret_cast<const char*>(buf), len, 0);
    }

    if (bw < 0) throw IOException("Socket write error");
    return bw;
}
