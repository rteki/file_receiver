#ifndef DIREXPCLIENT_H
#define DIREXPCLIENT_H
#include <iostream>
#include <fstream>
#include "directoryexplorer.h"


using std::string;


//Client specific directory explorer class
class DirExpClient: public DirExp
{


private:
    //Current downloads directory
    string g_save_dir;
public:
    //Set new downloads directory (g_save_dir)
    bool SetDownloadsDir(string);

    //Set current directory like downloads directory
    bool SetDownloadsDir();

    //Return current downloads directory
    string GetSaveDir();

    DirExpClient();
    ~DirExpClient();
};

#endif // DIREXPCLIENT_H
