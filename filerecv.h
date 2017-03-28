#ifndef FILERECV_H
#define FILERECV_H

#include <memory.h>
#include <iostream>
#include <fstream>

#include "socket.h"
#include "direxpclient.h"
#include "direxpserver.h"
#include "fileinfo.h"


using std::cout;
using std::cin;
using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
class FileRecv
{
private:

    //Socket for connection and data transfer
    Socket *g_socket;

    //Directory explorer for client side
    DirExpClient *g_dir_exp_c;

    //Directory explorer for server side
    DirExpServer *g_dir_exp_s;

    //Containing current server location for client
    string g_current_server_dir;

    //Buffer for data transfer
    char *g_buff;

    //Containing current program mode
    //SERVER - program works like server
    //CLIENT - program works like client
    //CCLIENT - program works like connected client
    char g_status;

    //Is in root of server
    bool g_server_root;

    //Containing line that entering
    string g_command;

    //Get program function from g_command
    string GetCommand(string);

    //Get program function parameter from g_command
    string GetParam(string);

    //Is path in added directories that accessible for client
    bool IsInAdded(string);

    //Basic Do method, geting to parameters function string and parameter string.
    //Choice of next Do method depends from first parameter.
    bool Do(string,string);

    //If Do("cd",string param)
    void DoCd(string);//Change current directory

    //If Do("ls"|"dir",param)
    void DoLs();//Show content of current directory

    //->Similar
    void DoSetD(string);//Choice of downloads directory     "setd @target_name"
    void DoShowD();//Show current downloads directory       "showd"
    void DoSize(string);//Show size of file                 "size @target_name"
    void DoFrees();//Show free disk space                   "frees"
    void DoDel(string);//Delete file                        "del|rm @target_name"
    void DoRd(string);//Remove directory                    "rd @target_name"
    void DoMd(string);//Create new directory                "md|mkdir|makedir @target_name"
    void DoDirList();//Show list of accessible directoris   "dirlist"
    void DoAdd(string);//Add to list of accessible dir.     "add @target"
    void DoExc(string);//Exclude from list                  "exc @target"
    void DoExcAll();//Exclude all directroies from list     "excall"
    void DoConnect();//Connect to server                    "connect"
    void DoBind(u_short);//Bind and start server            "bind @target_port"
    void DoDiscon();//Disconect from server                 "discon"
    void DoGet(string);//Download file                      "get @target_name"
    void PrintHelp();//Print help                              "help"
    //<-

    //Is ddd.ddd.ddd.ddd format (d - digit)
    bool IsIpFormat(string);

    //Is ddddd format
    bool IsPortFormat(string);

    //Disconnecting when connection is lost
    void DisconnectWhenLost();

public:

    //Run program
    void Run();

    FileRecv();
    FileRecv(FileRecv&);
    ~FileRecv();
};

#endif // FILERECV_H
