#include "socket.h"
#define LOCALHOST_STR "127.0.0.1"
#define DEFAULT_PORT 1024

#ifdef _WIN32
#define SOCK_VERSION 0x0202

Socket::Socket()
{
    g_sock=-1;
    g_listener=-1;
    WSAStartup(SOCK_VERSION,&g_init);
    g_sock_addr.sin_family=AF_INET;
    g_sock_addr.sin_addr.S_un.S_addr=inet_addr(LOCALHOST_STR);
    g_sock_addr.sin_port=htons(DEFAULT_PORT);
    g_connection_info.sin_family=AF_INET;
    g_connection_info.sin_addr.S_un.S_addr=inet_addr(LOCALHOST_STR);
    g_connection_info.sin_port=htons(DEFAULT_PORT);


}

Socket::~Socket(){
    WSACleanup();//turn off windows network library
}

bool Socket::Connect(string address,u_short port){
    g_sock_addr.sin_addr.S_un.S_addr=inet_addr(address.c_str());//differance between sockaddr_in structure in Windows and Lunux
    g_sock_addr.sin_port=htons(port);
    g_sock=socket(AF_INET,SOCK_STREAM,0);//create sockets, sets address family ant socket type

    if(connect(g_sock,(sockaddr*)&g_sock_addr,sizeof(sockaddr))<0)//connect
        return false;

    return true;
}

bool Socket::Bind(u_short port){
    g_listener=socket(AF_INET,SOCK_STREAM,0);

    if(g_listener<0)
        return false;

    g_sock_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    g_sock_addr.sin_port=htons(port);

    if(bind(g_listener,(sockaddr*)&g_sock_addr,sizeof(sockaddr))<0)//bind to port
        return false;

    return true;
}

bool Socket::Accept(){
    int size_of_sockaddr=sizeof(sockaddr_in);
    g_sock=accept(g_listener,(sockaddr*)&g_connection_info,&size_of_sockaddr);//accept and create socket;

    if(g_sock<0)
        return false;

    return true;
}

bool Socket::Close(){//close and destroy socket

    if(closesocket(g_sock)+closesocket(g_listener)==0)
        return true;

    return false;
}

#elif __linux__
Socket::Socket()
{
    g_sock=-1;
    g_listener=-1;

    g_sock_addr.sin_family=AF_INET;
    g_sock_addr.sin_addr.s_addr=inet_addr(LOCALHOST_STR);
    g_sock_addr.sin_port=htons(DEFAULT_PORT);

    g_connection_info.sin_family=AF_INET;
    g_connection_info.sin_addr.s_addr=inet_addr(LOCALHOST_STR);
    g_connection_info.sin_port=htons(DEFAULT_PORT);




}

bool Socket::Connect(string address,u_short port){
    g_sock_addr.sin_addr.s_addr=inet_addr(address.c_str());
    g_sock_addr.sin_port=htons(port);
    g_sock=socket(AF_INET,SOCK_STREAM,0);//create sockets, sets address family ant socket type

    if(connect(g_sock,(sockaddr*)&g_sock_addr,sizeof(sockaddr))<0)//connect
        return false;

    return true;
}

bool Socket::Bind(u_short port){
    g_listener=socket(AF_INET,SOCK_STREAM,0);
    if(g_listener<0)
        return false;

    g_sock_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    g_sock_addr.sin_port=htons(port);

    if(bind(g_listener,(sockaddr*)&g_sock_addr,sizeof(sockaddr))<0)//bind to port
        return false;

    return true;
}

bool Socket::Accept(){
    socklen_t size_of_sockaddr=sizeof(sockaddr_in);
    g_sock=accept(g_listener,(sockaddr*)&g_connection_info,&size_of_sockaddr);//accept and create socket;

    if(g_sock<0)
        return false;

    return true;
}

bool Socket::Close(){//close and destroy socket

    if(close(g_sock)+close(g_listener)==0)
        return true;

    return false;
}
#endif






bool Socket::Listen(u_short con_numb){

    if(listen(g_listener,con_numb)==0)
        return true;

    return false;
}



string Socket::GetIP(){

    return string(inet_ntoa(g_connection_info.sin_addr));//get IP in readable format
}

long Socket::Send(char *buff,size_t len){//send data

    return (long)send(g_sock,buff,len,0);

}

long Socket::Recv(char *buff,size_t len){//receive data

    return (long)recv(g_sock,buff,len,0);


}


