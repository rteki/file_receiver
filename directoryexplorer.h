#ifndef DIREXP_H
#define DIREXP_H
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <vector>
#include <fstream>
#include <memory.h>
#include <sys/stat.h>
#include "fileinfo.h"
#elif __linux__
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/statvfs.h>
#include <vector>
#include <fstream>
#include <memory.h>
#include "fileinfo.h"
#endif


//info about full path of directory and defined name
struct DirDefine{
    std::string dir_define;
    std::string dir_full_path;
};

//directory explorer class
class DirExp
{


private:
   //Current directory
    char *g_current_dir;
   //Start directory
    string g_start_dir;




public:

    DirExp();
    DirExp(DirExp&);
    ~DirExp();
    //Return start directory path
    std::string GetStartDir();

    //Return current directory path
    std::string GetCurrentDir();

    //Set new current directory
    bool SetCurrentDir(std::string);

    //Get vector wtith information about current directory content
    std::vector<FileInfo> GetContent();

    //Return available space on current disk
    unsigned long long GetAvailableSpace();

    //Create new directory
    bool MakeDir(std::string);

    //Remove file
    bool Remove(std::string);

    //Remove directory
    bool RemoveDir(std::string);
    unsigned long long GetFileSize(std::string);

    //Get slash what used by current opereting system in console
    char GetSystemSlash();

};




#endif // DIREXP_H
