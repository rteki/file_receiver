#ifndef DIREXPSERVER_H
#define DIREXPSERVER_H
#include <fstream>
#include <iostream>
#include <algorithm>
#include "directoryexplorer.h"
using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;


//Server specific directory explorer class
class DirExpServer : public DirExp
{


private:
    //Vector of defined directories
    //Containing directories accessible for client
    vector<DirDefine> g_added_directories;


public:

    //Correction for difine format
    static void DefinesFormatCorrection(string&);

    //Change current directory to defined by define name or full path
    bool GoTo(string);

    //Exclude directory from g_added_directories
    bool ExcludeFromAdded(string);

    //Clear g_added_directories
    void ExcludeFromAddedAll();

    //Return vector with defined directories
    vector<DirDefine> GetDefines();

    //Add directory to g_added_directories
    bool AddDirr(string);


    DirExpServer();
    ~DirExpServer();
};

#endif // DIREXPSERVER_H
