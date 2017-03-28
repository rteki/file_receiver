#include "direxpclient.h"


#define FILE_SAVE_DIR ".savedir"
DirExpClient::DirExpClient():DirExp()
{
    //if .savedir file is excist
    //read g_save_dir
    //else set downloads directory to application start directory
    std::ifstream file_read_save_dir;
    file_read_save_dir.open(GetStartDir()+GetSystemSlash()+FILE_SAVE_DIR,std::ios::in);
    if(file_read_save_dir.is_open()){
        std::getline(file_read_save_dir,g_save_dir);
        if(!SetCurrentDir(g_save_dir))
            g_save_dir=GetStartDir();
    }
    else{
        g_save_dir=GetStartDir();
    }

    file_read_save_dir.close();


}

bool DirExpClient::SetDownloadsDir(string downloads_dir){
    if(SetCurrentDir(downloads_dir)){
        g_save_dir=GetCurrentDir();
        return true;
    }
    return false;
}


bool DirExpClient::SetDownloadsDir(){
    g_save_dir=GetCurrentDir();
    return true;
}

string DirExpClient::GetSaveDir(){
    return g_save_dir;
}

DirExpClient::~DirExpClient(){
    //write to file g_save_dir, for next running
    std::ofstream file_write_save_dir;
    file_write_save_dir.open(GetStartDir()+GetSystemSlash()+FILE_SAVE_DIR,std::ios::trunc);
    file_write_save_dir<<g_save_dir;
    file_write_save_dir.close();


}




