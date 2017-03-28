#include "direxpserver.h"

#define ADDED_DIRS_FILE_NAME ".accessibledirs"


DirExpServer::DirExpServer()
{
    //If .accessibledirs is excist
    //Read defines and paths
    //Else set one accessible directory: aplication start directory
    DirDefine dir;
    ifstream f_access_files;
    SetCurrentDir(GetCurrentDir());

    f_access_files.open(GetStartDir()+GetSystemSlash()+ADDED_DIRS_FILE_NAME,std::fstream::in);

    if(f_access_files.is_open()){

        while(!f_access_files.eof()){
            std::getline(f_access_files,dir.dir_define);
            std::getline(f_access_files,dir.dir_full_path);
            DefinesFormatCorrection(dir.dir_define);
            g_added_directories.push_back(dir);
        }
    }
    else
    {
        string current_dir = GetCurrentDir();
        dir.dir_full_path=current_dir;
        DefinesFormatCorrection(current_dir);
        dir.dir_define=current_dir;
        g_added_directories.push_back(dir);
    }
    f_access_files.close();
}

void DirExpServer::DefinesFormatCorrection(string& str){
    while(str[str.size()-1]=='\\'||str[str.size()-1]=='/')//can't change to GetSystemSlash() because Windows is supporting both formats
        str.erase(str.size()-1,1);

    int i=str.size();
    while(str[i]!='\\' && str[i]!='/' && i>=0)
        i--;
    if(i>=0)
        str.erase(0,i+1);


}

vector<DirDefine> DirExpServer::GetDefines(){
    return g_added_directories;
}

bool DirExpServer::AddDirr(std::string dir_name){
    DirDefine dir;
    if(SetCurrentDir(dir_name)){//if it's directory
        dir.dir_define=dir_name;
        dir.dir_full_path=GetCurrentDir();
        SetCurrentDir("..");
        DefinesFormatCorrection(dir.dir_define);
        g_added_directories.push_back(dir);
        return true;
    }
    return false;
}

bool DirExpServer::ExcludeFromAdded(string target){
    for(unsigned i=0;i<g_added_directories.size();i++){

        if(g_added_directories[i].dir_full_path==target||g_added_directories[i].dir_define==target){
            g_added_directories.erase(g_added_directories.begin()+i);
            return true;
        }
    }


    return false;
}


//find define or path inf g_added_directories and remove it
bool DirExpServer::GoTo(string target){
    for(auto i:g_added_directories){
        if(i.dir_define==target||i.dir_full_path==target){
            if(SetCurrentDir(i.dir_full_path))
                return true;
            else
                return false;

        }
    }
    return false;
}

//just clear all accessible for client directories
void DirExpServer::ExcludeFromAddedAll(){
    g_added_directories.clear();
}


//Write to file list of accessible for client directories
//If list is empty, just delete .accessibledirs
DirExpServer::~DirExpServer(){
    ofstream f_access_files;

    f_access_files.open(GetStartDir()+GetSystemSlash()+ADDED_DIRS_FILE_NAME,std::fstream::trunc);


    if(g_added_directories.empty()){
        f_access_files.close();

        Remove(GetStartDir()+GetSystemSlash()+ADDED_DIRS_FILE_NAME);

    }
    for(auto i:g_added_directories){
        f_access_files<<i.dir_define<<endl;
        f_access_files<<i.dir_full_path;
        if(i.dir_full_path!=(*(g_added_directories.end()-1)).dir_full_path)
            f_access_files<<endl;
    }
    f_access_files.close();
}

