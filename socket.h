#ifndef SOCKET_H
#define SOCKET_H
#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <stdio.h>

#elif _WIN32
#include <winsock2.h>
#include <string>
#include <stdio.h>

#endif


#ifdef __linux__
typedef int SOCKET;
#endif

using std::string;

class Socket
{
private:
    //socket for data transfer
    SOCKET g_sock;
    //listener socket of server side
    SOCKET g_listener;
#ifdef _WIN32
    WSADATA g_init;
#endif
    //socket adress
    sockaddr_in g_sock_addr;

    //contain address of connected client
    sockaddr_in g_connection_info;
public:

    //connect to @ip,@port
    bool Connect(string,u_short);

    //bind to @port
    bool Bind(u_short);

    //listen for @number_of_clients
    bool Listen(u_short);

    //accept connection
    bool Accept();

    //get g_sock_addr ip
    string GetIP();

    //send @buffer,@size_of_trans_data
    long Send(char*,size_t);

    //receive @buffer,@size_of_recv_data
    long Recv(char*,size_t);

    //destroy socket
    bool Close();
    Socket();
#ifdef _WIN32
    ~Socket();
#endif
};


#endif // SOCKET_H
