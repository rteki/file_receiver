#ifndef FILEINFO_H
#define FILEINFO_H
#include <iostream>
using std::string;
using std::ostream;
using std::endl;
using std::getline;


//Containing information about file
class FileInfo
{
private:

    //File type <dir> or <file>
    string g_file_type;

    //File name
    string g_file_name;

    //File size
    unsigned long long g_file_size;
public:
    //Set g_file_type
    void SetFileType(string);

    //Set g_file_size
    void SetFileSize(unsigned long long);

    //Set g_file_name
    void SetFileName(string);

    //Return g_file_type
    string GetFileType();

    //Return g_file_size
    unsigned long long GetFileSize();

    //Return g_file_name
    string GetFileName();

    //Friend method for printing to console
    friend ostream& operator<<(ostream &os,FileInfo file);

    FileInfo();
    FileInfo(string file_name,string file_type,unsigned long long file_size);

};

#endif // FILEINFO_H
