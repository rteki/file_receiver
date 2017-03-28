#include "fileinfo.h"

#define FILE_TYPE "<file>"
#define DIR_TYPE  " <dir>"

//Easey container for information of file

FileInfo::FileInfo(string file_name,string file_type,unsigned long long file_size)
{
    g_file_name=file_name;
    g_file_type=file_type;
    g_file_size=file_size;
}

FileInfo::FileInfo()
{
    g_file_name="\0";
    g_file_type=FILE_TYPE;
    g_file_size=0;
}

void FileInfo::SetFileName(string file_name){
    g_file_name=file_name;
    return;
}

void FileInfo::SetFileSize(unsigned long long file_size){
    g_file_size=file_size;
    return;
}

void FileInfo::SetFileType(string file_type){
    g_file_type=file_type;
    return;
}

string FileInfo::GetFileName(){
    return g_file_name;
}

string FileInfo::GetFileType(){
    return g_file_type;
}

unsigned long long FileInfo::GetFileSize(){
    return g_file_size;
}

ostream& operator<<(ostream &os,FileInfo file){
    if(file.GetFileType()==DIR_TYPE)
        os<<'\t'<<file.GetFileType()<<'\t'<<file.GetFileName()<<"\t";


    if(file.GetFileType()==FILE_TYPE)
        os<<'\t'<<file.GetFileType()<<'\t'<<file.GetFileName();
    return os;
}


