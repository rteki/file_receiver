#include "directoryexplorer.h"
//define thanks difference between Windows and Linux formats


#define MAX_DIR_PATH 260

#ifdef _WIN32
DirExp::DirExp(){
    g_current_dir=new char[MAX_DIR_PATH];
    GetCurrentDirectoryA(MAX_DIR_PATH,g_current_dir);
    g_start_dir=GetCurrentDir();
}

std::string DirExp::GetCurrentDir(){
    GetCurrentDirectoryA(MAX_PATH,g_current_dir);//working with std::string not std::wstring
    return  std::string(g_current_dir);

}

bool DirExp::SetCurrentDir(std::string target_dir){
    return  SetCurrentDirectoryA(target_dir.c_str());
}

std::vector<FileInfo> DirExp::GetContent(){
    GetCurrentDir();
    std::vector<FileInfo> file_list;//vector with files info structures
    FileInfo file_info;
    HANDLE dir;
    WIN32_FIND_DATAA file_data;
    dir = FindFirstFileA((string(g_current_dir) + "\\*").c_str(), &file_data);

    do {
        if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0){//if file data is directory
            file_info.SetFileType(" <dir>");//set file_info type field to <dir>
            file_info.SetFileName(std::string(file_data.cFileName)+GetSystemSlash());//set file name
        }
        else{//if is not directory
            file_info.SetFileType("<file>");//set fil_info type field to <file>
            file_info.SetFileName(file_data.cFileName);//set file name
        }

        file_list.push_back(file_info);//add file information to vector
    } while (FindNextFileA(dir, &file_data));//whie is finding next file in directory

    FindClose(dir);//close searching

    return file_list;//return directory content
}


unsigned long long DirExp::GetAvailableSpace(){//get avalible disk space
    unsigned long long free_bytes_available=0;
    GetDiskFreeSpaceExA(NULL,(PULARGE_INTEGER)&free_bytes_available,NULL,NULL);
    return free_bytes_available;
}

bool DirExp::MakeDir(std::string path){//create new directory
    if(mkdir(path.c_str())==-1)
        return false;

    return true;
}


unsigned long long DirExp::GetFileSize(std::string file_name){
    struct _stat64 buf;

    if((_stat64(file_name.c_str(),&buf)==-1))
        return 0;
    unsigned long long file_size=buf.st_size;
    return file_size;
}


char DirExp::GetSystemSlash(){
    return '\\';
}

#elif __linux__
DirExp::DirExp(){
    g_current_dir=new char[MAX_DIR_PATH];
    getwd(g_current_dir);
    g_start_dir=GetCurrentDir();
}

std::string DirExp::GetCurrentDir(){
    getwd(g_current_dir);
    return  std::string(g_current_dir);
}

bool DirExp::SetCurrentDir(std::string target_dir){
    if(chdir(target_dir.c_str())==-1)
        return false;

    return true;

}


std::vector<FileInfo> DirExp::GetContent(){
    GetCurrentDir();
    std::vector<FileInfo> file_list;//vector with files info structures
    FileInfo file_info;
    std::string full_path;//containing full path to file
    DIR *dir;//pointer to directory
    dirent *ent;//pointer to contained element
    class stat st;//for file testing
    dir=opendir(g_current_dir);
    while ((ent=readdir(dir))!=NULL) {//while not end of directory
        file_info.SetFileName(ent->d_name);
        full_path=g_current_dir+GetSystemSlash()+file_info.GetFileName();//create full path
        stat(full_path.c_str(),&st);//initialization of st to testing
        if( (st.st_mode & S_IFDIR)!=0 ){//If it's directory
            file_info.SetFileType(" <dir>");
            file_info.SetFileName(std::string(ent->d_name)+GetSystemSlash());
        }
        else{
            file_info.SetFileType("<file>");
            file_info.SetFileName(ent->d_name);
        }
        file_list.push_back(file_info);//add to vector
    }
    closedir(dir);//close directory poiner



    return file_list;//return directory content


}


unsigned long long DirExp::GetAvailableSpace(){//get avalible disk space
    unsigned long long free_bytes_available=0;
    struct statvfs disk_status;//file system status structure
    if(statvfs(GetCurrentDir().c_str(),&disk_status)==-1)//if cant get status
        return 0;

    free_bytes_available=disk_status.f_bsize*disk_status.f_bfree;//multiply free memmory blocks to size of this blocks


    return free_bytes_available;
}

bool DirExp::MakeDir(std::string path){//create new directory

    if(mkdir(path.c_str(),S_IRWXU)==-1)
        return false;

    return true;
}

unsigned long long DirExp::GetFileSize(std::string file_name){
    struct stat64 buf;

    if( ((stat64(file_name.c_str(),&buf))==-1) )
        return 0;

    unsigned long long file_size=buf.st_size;
    return file_size;
}

char DirExp::GetSystemSlash(){
    return '/';
}

#endif




DirExp::DirExp(DirExp & directory_explorer){
    memcpy(this->g_current_dir,directory_explorer.g_current_dir,MAX_DIR_PATH);
    this->g_start_dir=directory_explorer.g_start_dir;
}


DirExp::~DirExp(){
    delete [] g_current_dir;
}

bool DirExp::Remove(std::string target){//remove file

    if(remove(target.c_str())==-1)
        return false;

    return true;
}

bool DirExp::RemoveDir(std::string target){//remove directory

    if(rmdir(target.c_str())==-1)
        return false;

    return true;
}



string DirExp::GetStartDir(){
    return g_start_dir;
}



