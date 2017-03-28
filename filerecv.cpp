#include "filerecv.h"

#define SERVER 's'
#define CLIENT 'c'
#define CCLIENT 'x'
#define BUFF_SIZE 512
#define MBYTE 1048576
#define KBYTE 1024

FileRecv::FileRecv()
{
    g_socket=new Socket();
    g_dir_exp_s=new DirExpServer();
    g_dir_exp_c=new DirExpClient();
    g_buff=new char[BUFF_SIZE];

    g_status=CLIENT;
    g_current_server_dir="server>";
    g_server_root=true;
    g_command='\0';

}


FileRecv::FileRecv(FileRecv & file_receiver){
    *g_socket=*file_receiver.g_socket;
    *g_dir_exp_s=*file_receiver.g_dir_exp_s;
    *g_dir_exp_c=*file_receiver.g_dir_exp_c;
    memcpy(g_buff,file_receiver.g_buff,BUFF_SIZE);
    g_status=file_receiver.g_status;
    g_current_server_dir=file_receiver.g_current_server_dir;
    g_server_root=file_receiver.g_server_root;
    g_command=file_receiver.g_command;

}

FileRecv::~FileRecv(){
    g_socket->Send("discon\0",BUFF_SIZE);//if exit - send to server command for disconaction
    delete g_socket;
    delete g_dir_exp_c;
    delete g_dir_exp_s;
    delete [] g_buff;


}

string FileRecv::GetCommand(string line){
    while(line[0]==' ')
        line.erase(0,1);
    unsigned i=0;
    while(line[i]!=' '&&i!=line.size())
        i++;

    line.erase(i,line.size()-i);
    return line;
}

bool FileRecv::IsInAdded(string str){//checking for server directories access control
    vector<DirDefine>defines=g_dir_exp_s->GetDefines();//get accessible directories list
    for(auto i:defines){
        if(str.find(i.dir_full_path)!=string::npos)//if defined full path is part of path of current directory
            return true;
    }
    return false;
}



string FileRecv::GetParam(string line){
    while(line[0]==' ')
        line.erase(0,1);
    unsigned i=0;
    while( !(line[i]==' '&&line[i+1]!=' ')&&i<line.size() )
        i++;


    line.erase(0,i+1);

    while(line[line.size()-1]==' ')
        line.erase(line.size()-1,1);
    return line;
}

void FileRecv::DisconnectWhenLost(){
    g_status=CLIENT;
    g_socket->Close();
    cout<<"\n\tConnection is lost!"<<endl;
    cout<<"\tDisconnected!"<<endl;
}

void FileRecv::DoCd(string path){
    //if application mode is CLIENT - just change current directory
    if(g_status==CLIENT){
        if(!g_dir_exp_c->SetCurrentDir(path))
            cout<<"\tCan't change directory!"<<endl;
    }

    //if application mode is connected client-
    //send to server a command "cd" and parameter @path
    //get from server @g_current_server_dir
    if(g_status==CCLIENT){
        string line="cd "+path;
        memcpy(g_buff,line.c_str(),BUFF_SIZE);
        g_socket->Send(g_buff,BUFF_SIZE);
        long connection_test=g_socket->Recv(g_buff,BUFF_SIZE);
        if(connection_test<=0){
            DisconnectWhenLost();
            return;
        }
        string current_server_dir(g_buff);
        g_current_server_dir="server>"+current_server_dir;
    }

    //if SERVER mode
    if(g_status==SERVER){
        if(g_server_root){//if in server root
            if(g_dir_exp_s->GoTo(path)){//goto defined directory
                g_server_root=false;//and change g_server_root to false
                memcpy(g_buff,g_dir_exp_s->GetCurrentDir().c_str(),BUFF_SIZE);
                g_socket->Send(g_buff,BUFF_SIZE);//send current location to client
            }
            else{//if can't goto
                g_server_root=true;
                g_socket->Send("\0",BUFF_SIZE);//send that we not change location
            }
        }
        else{//if not in server root
            if((g_dir_exp_c->GetCurrentDir().size())==3&&path==".."){
                g_server_root=true;
                g_socket->Send("\0",BUFF_SIZE);
                return;
            }

            g_dir_exp_s->SetCurrentDir(path);//change current dir
            if(IsInAdded(g_dir_exp_s->GetCurrentDir())){//if client have access
                memcpy(g_buff,g_dir_exp_s->GetCurrentDir().c_str(),BUFF_SIZE);
                g_socket->Send(g_buff,BUFF_SIZE);//send current position
            }
            else{//if not
                g_server_root=true;//goto root
                g_socket->Send("\0",BUFF_SIZE);//send root current location
            }
        }

    }
    return;
}


void FileRecv::DoLs(){

    //if CLIENT mode just show content of directory
    if(g_status==CLIENT){
        cout<<"Directory of "<<g_dir_exp_c->GetCurrentDir()<<endl;
        vector<FileInfo> files_list=g_dir_exp_c->GetContent();
        for(auto i:files_list){
            cout<<i<<endl;
        }
    }

    //if connected client mode
    //send request to server
    //receive and write to console content of current directory on server
    if(g_status==CCLIENT){
        char line[]="ls";
        g_socket->Send(line,BUFF_SIZE);
        int connection_test=g_socket->Recv(g_buff,BUFF_SIZE);
        if(connection_test<=0){
            DisconnectWhenLost();
            return;
        }
        unsigned number=*(unsigned*)g_buff;
        for(unsigned i=0;i<number;i++){
            connection_test=g_socket->Recv(g_buff,BUFF_SIZE);
            if(connection_test<=0){
                DisconnectWhenLost();
                return;
            }
            cout<<"\t"<<g_buff;
            connection_test=g_socket->Recv(g_buff,BUFF_SIZE);
            if(connection_test<=0){
                DisconnectWhenLost();
                return;
            }
            cout<<"\t"<<g_buff<<endl;
        }
    }

    //if server mode
    //get content of directory
    //and sending to client
    if(g_status==SERVER){
        if(g_server_root){
            vector<DirDefine> dirs=g_dir_exp_s->GetDefines();
            unsigned int count=0;
            for(auto i:dirs){
                count++;
            }
            g_socket->Send((char*)&count,BUFF_SIZE);
            for(auto i:dirs){
                memcpy(g_buff," <dir>\0",BUFF_SIZE);
                g_socket->Send(g_buff,BUFF_SIZE);
                memcpy(g_buff,i.dir_define.c_str(),BUFF_SIZE);
                g_socket->Send(g_buff,BUFF_SIZE);

            }
        }
        else{
            vector<FileInfo> files=g_dir_exp_s->GetContent();
            unsigned int count=0;
            for(auto i:files)
                count++;
            g_socket->Send((char*)&count,BUFF_SIZE);
            for(auto i:files){
                memcpy(g_buff,i.GetFileType().c_str(),i.GetFileType().size()+1);
                g_socket->Send(g_buff,BUFF_SIZE);
                memcpy(g_buff,i.GetFileName().c_str(),i.GetFileName().size()+1);
                g_socket->Send(g_buff,BUFF_SIZE);
            }
        }
    }
    return;
}

//Set downloads directory
//possible only in CLIENT mode and CCLIENT mode
void FileRecv::DoSetD(string path){
    if(g_status==CLIENT||g_status==CCLIENT){
        if(g_dir_exp_c->SetDownloadsDir(path))
            cout<<'\t'<<'"'<<path<<'"'<<" have been seted for downloads!"<<endl;
        else
            cout<<"Can't set "<<'"'<<path<<'"'<<" for downloads"<<endl;
    }



    return;
}

//Show current downloads directory
void FileRecv::DoShowD(){
    if(g_status==CLIENT||g_status==CCLIENT){
        cout<<"\tDownloads directory:"<<endl;
        cout<<"\t\t"<<g_dir_exp_c->GetSaveDir()<<endl;
    }

    return;
}

void FileRecv::DoSize(string file){
    //if client mode
    //just get size of @file on client's hard disk
    if(g_status==CLIENT){
        unsigned long long file_size=g_dir_exp_c->GetFileSize(file);
        if(file_size==0){
            cout<<"\tCan't get size of \""<<file<<'"'<<endl;
            return;
        }
        cout<<"\tSize of "<<'"'<<file<<'"'<<" is "<<file_size<<" bytes."<<endl;
    }

    //if connected client mode
    //send request for server
    //and receive size of requested file
    if(g_status==CCLIENT){
        string line="size "+file;
        memcpy(g_buff,line.c_str(),BUFF_SIZE);
        g_socket->Send(g_buff,BUFF_SIZE);
        long connection_test=g_socket->Recv(g_buff,BUFF_SIZE);
        if(connection_test<=0){
            DisconnectWhenLost();
            return;
        }
        unsigned long long file_size;
        file_size=*(unsigned long long*)g_buff;
        if(file_size>0)
            cout<<"\tSize of "<<'"'<<file<<'"'<<" is "<<file_size<<" bytes."<<endl;
        else
            cout<<"Can't get size of \""<<file<<'"'<<endl;
    }

    //get file size and send it to client
    if(g_status==SERVER){
        unsigned long long file_size=g_dir_exp_c->GetFileSize(file);

        memcpy(g_buff,&file_size,BUFF_SIZE);
        g_socket->Send(g_buff,BUFF_SIZE);
    }
    return;

}

void FileRecv::DoFrees(){
    //if CLIENT - show free disk space of current disk
    //if CCLIENT - show free disk space where located downloads directory
    if(g_status==CLIENT)
        cout<<"\tFree space of current disk is "<<g_dir_exp_c->GetAvailableSpace()<<" bytes."<<endl;
    if(g_status==CCLIENT){
        g_dir_exp_c->SetCurrentDir(g_dir_exp_c->GetSaveDir());
        cout<<"\tFree space of downloads disk is "<<g_dir_exp_c->GetAvailableSpace()<<" bytes."<<endl;
    }
}



void FileRecv::DoDel(string target){
    //just delete file
    if(g_status==CLIENT){
        if(g_dir_exp_c->Remove(target))
            cout<<'\t'<<'"'<<target<<'"'<<" have been removed!"<<endl;
        else
            cout<<"\tCan't remove "<<'"'<<target<<'"'<<endl;
    }
    if(g_status==CCLIENT){
        cout<<"Can't delete file. You are connect to server."<<endl;
    }
    return;
}

void FileRecv::DoRd(string target){
    //just remove directory
    if(g_status==CLIENT){
        if(g_dir_exp_c->RemoveDir(target))
            cout<<'\t'<<'"'<<target<<'"'<<" have been removed!"<<endl;
        else
            cout<<"\tCan't remove "<<'"'<<target<<'"'<<endl;
    }
    if(g_status==CCLIENT){
        cout<<"Can't delete directory. You are connect to server."<<endl;
    }
}

void FileRecv::DoMd(string dir){
    //create directory
    if(g_status==CLIENT){
        if(g_dir_exp_c->MakeDir(dir))
            cout<<"\tCreated "<<'"'<<dir<<'"'<<" directory"<<endl;
        else
            cout<<"Can't create directory!"<<endl;

    }
    if(g_status==CCLIENT){
        cout<<"Can't make directory. You are connect to server."<<endl;
    }
    return;
}

void FileRecv::DoDirList(){
    //show accessible for client directory list
    if(g_status==CLIENT||g_status==CCLIENT){
        vector<DirDefine> dir_list=g_dir_exp_s->GetDefines();
        cout<<"Added directories:"<<endl;
        for(auto i:dir_list){
            cout<<"\t <dir>\t"<<i.dir_define<<'\t'<<i.dir_full_path<<endl;
        }
    }
    return;
}

void FileRecv::DoAdd(string target){
    //add to accessible directories list @target
    if(g_status==CLIENT){
        if(g_dir_exp_s->AddDirr(target))
            cout<<'\t'<<'"'<<target<<'"'<<" is added!"<<endl;
        else
            cout<<"Can't add "<<'"'<<target<<'"'<<endl;
    }
    if(g_status==CCLIENT){
        cout<<"Can't add directory. You are connect to server."<<endl;
    }
    return;
}

void FileRecv::DoExc(string target){
    //exclude from accessible directories list @target
    if(g_status==CLIENT){
        if(g_dir_exp_s->ExcludeFromAdded(target))
            cout<<'\t'<<'"'<<target<<'"'<<" have been excluded!"<<endl;
        else
            cout<<"Cant't exclude "<<'"'<<target<<'"'<<'.'<<endl;
    }
    if(g_status==CCLIENT){
        cout<<"Can't exclude directory. You are connect to server."<<endl;
    }
    return;
}



void FileRecv::DoExcAll(){
    //exclude all defined directories
    if(g_status==CLIENT){
        g_dir_exp_s->ExcludeFromAddedAll();
        cout<<"All directories have been excluded!"<<endl;
    }
    if(g_status==CCLIENT){
        cout<<"Can't exclude. You are connect to server."<<endl;
    }
    return;
}


void FileRecv::DoConnect(){
    if(g_status==CCLIENT){
        cout<<"Disconect at first."<<endl;
    }

    if(g_status==CLIENT){
        string address;
        string s_port;
        g_current_server_dir="server>";

        cout<<"\tPrint IP: ";
        std::getline(cin,address);
        cout<<"\tPrint Port: ";
        std::getline(cin,s_port);

        if( !(IsIpFormat(address)) ){
            cout<<"Wrong IP or port!"<<endl;
            cout<<"IP format: DDD.DDD.DDD.DDD (D - digit)."<<endl;
            cout<<"Port is number in [1024;65535]"<<endl;
            return;

        }

        u_long port=strtoul(s_port.c_str(),NULL,0);
        if(port<1024||port>65535||(!IsPortFormat(s_port))){
            cout<<"Wrong IP or port!"<<endl;
            cout<<"IP format: DDD.DDD.DDD.DDD (D - digit)."<<endl;
            cout<<"Port is number in [1024;65535]"<<endl;
            return;
        }



        cout<<"Connecting..."<<endl;
        if(g_socket->Connect(address,port)){//if connected to server
            cout<<"Waiting for answer from "<<address<<':'<<port<<"..."<<endl;
            long connection_test=g_socket->Recv(g_buff,BUFF_SIZE);
            if(connection_test<=0){
                DisconnectWhenLost();
                return;
            }
            if(g_buff[0]=='1'){     //if connection accepted
                cout<<"Connected to "<<address<<':'<<port<<'.'<<endl;
                g_status=CCLIENT;   //change to conected client mode


            }
            else{
                cout<<"Connect disabled!"<<endl;
                g_socket->Close();//destroy socket
            }
        }
        else
            cout<<"Can't connect!"<<endl;
    }
    return;

}

void FileRecv::DoBind(u_short port){
    if(g_status==CLIENT){
        g_server_root=true;
        g_status=SERVER;
        if(!g_socket->Bind(port))
            cout<<"Can't bind"<<endl;
        if(!g_socket->Listen(1))
            cout<<"Can't listen"<<endl;

        cout<<"Waiting for client..."<<endl;
        g_socket->Accept();//accept connection
        cout<<"Client have connected."<<endl;
        string accept;
        cout<<"Connecting from: "<<g_socket->GetIP()<<endl;
        cout<<"Accepting (yes/no): ";
        std::getline(cin,accept);
        while(!((accept=="yes")||(accept=="no"))){//while not correct command
            cout<<"Connecting from: "<<g_socket->GetIP()<<endl;
            cout<<"Accepting (yes/no): ";
            std::getline(cin,accept);

        }

        if(accept=="no"){
            g_buff[0]='0';
            g_socket->Send(g_buff,BUFF_SIZE);//send "no" answer
            cout<<"Disable"<<endl;
            g_socket->Close();
        }

        if(accept=="yes"){
            g_buff[0]='1';
            g_socket->Send(g_buff,BUFF_SIZE);//send "yes" answer

            cout<<"Server is started!"<<endl;
            //while client not send "disconecton" message
            while(g_status==SERVER){



                long connection_test=g_socket->Recv(g_buff,BUFF_SIZE);

                if(connection_test<=0){
                    DisconnectWhenLost();
                    return;
                }

                if(connection_test>0){
                    string command_line(g_buff);
                    Do(GetCommand(command_line),GetParam(command_line));//Request processing
                }
            }
            g_status=CLIENT;
            cout<<"Disconected!"<<endl;
            g_socket->Close();//destroy socket
        }

    }
    return;
}

void FileRecv::DoDiscon(){
    if(g_status==CCLIENT){
        char discon_massage[]="discon";
        g_socket->Send(discon_massage,BUFF_SIZE);//send disconection message to server
        g_status=CLIENT;//change status
        g_socket->Close();//destroy socket
    }

    if(g_status==SERVER){
        g_status=CLIENT;//just change status to CLIENT and request processing is stop
    }
    return;
}

void FileRecv::DoGet(string target_file){
    //send request to server
    if(g_status==CCLIENT){
        string line="get "+target_file;
        memcpy(g_buff,line.c_str(),line.size()+1);
        g_socket->Send(g_buff,BUFF_SIZE);
        long connection_test=g_socket->Recv(g_buff,BUFF_SIZE);//receive file size
        if(connection_test<=0){
            DisconnectWhenLost();
            return;
        }
        unsigned long long file_size=*((unsigned long long*)g_buff);
        g_dir_exp_c->SetCurrentDir(g_dir_exp_c->GetSaveDir());//set current directory to downloads directory

        if(file_size>g_dir_exp_c->GetAvailableSpace()){//check for enogh space on disk
            cout<<"\tIs not enogh disk space"<<endl;
            return;
        }

        if(file_size>0){
            ofstream file;
            long pack_size=0;
            char *buff;
            if(file_size>=MBYTE){
                buff=new char[KBYTE*10];
                pack_size=KBYTE*10;
            }
            else{
                buff=new char[BUFF_SIZE];
                pack_size=BUFF_SIZE;
            }

            string path=g_dir_exp_c->GetSaveDir()+g_dir_exp_c->GetSystemSlash()+target_file;//path to file on client side
            file.open(path,std::ios::trunc|std::ios::binary);
            long trans_data;
            unsigned long long total=0;
            unsigned long long show_progress_step=(file_size/100)/pack_size;//how many "packets" is equal to 1% of file
            unsigned long long walker=0;//counter of received "packets"

            if(show_progress_step==0)
                show_progress_step=1;//Becouse we need at least one "packet" for file transfer.

            while(total<file_size){//receving and writing file
                trans_data=g_socket->Recv(buff,pack_size);

                if(trans_data<=0){
                    DisconnectWhenLost();
                    delete [] buff;
                    return;
                }
                total+=trans_data;
                walker++;

                if(total>=file_size){//if total receved data is more then file size

                    trans_data-=(total-file_size);//cut excess bytes
                    total-=(total-file_size);
                    cout<<"Downloading: "<<total<<" "<<g_dir_exp_c->GetSystemSlash()<<" "<<file_size<<" bytes ("<<int((double(total)/double(file_size))*double(100))<<"%)\r";
                    file.write(buff,trans_data);//write to file
                    break;
                }

                if(walker%show_progress_step==0)
                    cout<<"Downloading: "<<total<<" "<<g_dir_exp_c->GetSystemSlash()<<" "<<file_size<<" bytes ("<<int((double(total)/double(file_size))*double(100))<<"%)\r";
                file.write(buff,trans_data);//write to file
            }
            cout<<endl;
            delete [] buff;
            file.close();//close file
        }
    }

    //if SERVER mode
    //reading and sending of file
    if(g_status==SERVER){
        unsigned long long file_size=g_dir_exp_s->GetFileSize(target_file);
        memcpy(g_buff,&file_size,sizeof(unsigned long long));
        g_socket->Send(g_buff,BUFF_SIZE);
        if(file_size>0){
            ifstream file;

            long pack_size=0;
            char *buff;
            if(file_size>=MBYTE){
                buff=new char[KBYTE*10];
                pack_size=KBYTE*10;
            }
            else{
                buff=new char[BUFF_SIZE];
                pack_size=BUFF_SIZE;
            }

            file.open(target_file,std::ios::binary|std::ios::in);//in binary mode
            while(!file.eof()){//while not end of file
                file.read(buff,pack_size);//read
                g_socket->Send(buff,pack_size);//and send

            }
            delete [] buff;
            file.close();//close file
        }
    }
}

bool FileRecv::IsIpFormat(string ip){
    u_short dots_count=0;
    for(unsigned i=0;i<ip.size();i++){
        if(isdigit(ip[i])&&ip[i+1]=='.'&&isdigit(ip[i+2]))
            dots_count++;
    }

    if(dots_count!=3)
        return false;

    for(auto i:ip){
        if(!isdigit(i)&&i!='.')
            return false;
    }

    if(ip.size()>15||ip.size()<7)
        return false;

    u_short digits_count=0;
    for(unsigned i=0;i<=ip.size();i++){

        if(isdigit(ip[i]))
            digits_count++;

        if(i==ip.size()||ip[i]=='.'){
            if(digits_count>3)
                return false;
            digits_count=0;
        }


    }



    return true;

}


bool FileRecv::IsPortFormat(string port){
    for(auto i:port){
        if(!isdigit(i))
            return false;
    }
    return true;
}

//HELP FUNCTION
void FileRecv::PrintHelp(){
    cout<<"\tconnect"<<endl;
    cout<<"\t\tPrint \"connect\" and follow instructions for connecting to the server"<<endl<<endl;
    cout<<"\tbind"<<endl;
    cout<<"\t\tPrint \"bind <port:0-65536>\" for start server and listen from port. \n\t\tExample: bind 5000"<<endl<<endl;
    cout<<"\tcd"<<endl;
    cout<<"\t\tPrint \"cd <path>\" for change current directory (also you can print directory in current location)"<<endl<<endl;
    cout<<"\tdir|ls"<<endl;
    cout<<"\t\tPrint \"dir|ls\" for watching content of current directory"<<endl<<endl;
    cout<<"\tsetd"<<endl;
    cout<<"\t\tPrint \"setd <path>\" for seting of downloads directory"<<endl<<endl;
    cout<<"\tshowd"<<endl;
    cout<<"\t\tShow current downloads directory"<<endl<<endl;
    cout<<"\trm|del"<<endl;
    cout<<"\t\tPrint \"rm|del <filename>\" for removing of file"<<endl<<endl;
    cout<<"\trd"<<endl;
    cout<<"\t\tPrint \"rd <dirname>\" for removing of directory"<<endl<<endl;
    cout<<"\tmd|mkdir|makedir"<<endl;
    cout<<"\t\tPrint \"md|mkdir|makedir <dirname>\" for creating directory"<<endl<<endl;
    cout<<"\tget"<<endl;
    cout<<"\t\tPrind \"get <filename>\" for downloading file from server"<<endl<<endl;
    cout<<"\tdirlist"<<endl;
    cout<<"\t\tPrint \"dirlist\" for showing list of added directories"<<endl<<endl;
    cout<<"\tadd"<<endl;
    cout<<"\t\tPrint \"add <dirname>\" for adding directory to list of visible directories for client"<<endl<<endl;
    cout<<"\texc"<<endl;
    cout<<"\t\tPrint \"exc <dirname>|<dirdef>\" for excluding of directories from list"<<endl<<endl;
    cout<<"\texcall"<<endl;
    cout<<"\t\tPrint \"excall\" for excluding of all directories from list"<<endl<<endl;
    cout<<"\tsize"<<endl;
    cout<<"\t\tPrint \"size <filename>\" for geting size of file"<<endl<<endl;
    cout<<"\tfrees"<<endl;
    cout<<"\t\tPrint \"frees\" for watching free space on disk"<<endl<<endl;
    cout<<"\tdiscon"<<endl;
    cout<<"\t\tPrint \"discon\" for disconeting from the server"<<endl<<endl;
    cout<<"\texit"<<endl;
    cout<<"\t\tPrint \"exit\" for exit"<<endl<<endl;
}


//Basic commands processor method
bool FileRecv::Do(string cmd,string param){

    if(cmd=="connect"){
        DoConnect();
        return true;
    }
    if(cmd=="bind"){
        if(!IsPortFormat(param))
            return false;

        u_long port=strtoul(param.c_str(),NULL,0);
        if(port>=1024&&port<=65535){
            DoBind(port);
            return true;

        }
        else
            cout<<"Wrong port! Choose another in [1024;65535]."<<endl;
    }

    if(cmd=="cd"){
        DoCd(param);
        return true;
    }

    if(cmd=="dir"||cmd=="ls"){
        DoLs();
        return true;
    }

    if(cmd=="setd"){
        DoSetD(param);
        return true;
    }

    if(cmd=="rm"||cmd=="del"){
        DoDel(param);
        return true;
    }

    if(cmd=="rd"){
        DoRd(param);
        return true;
    }

    if(cmd=="mkdir"||cmd=="md"||cmd=="makedir"){
        DoMd(param);
        return true;
    }

    if(cmd=="get"){
        DoGet(param);
        return true;
    }

    if(cmd=="dirlist"){
        DoDirList();
        return true;
    }

    if(cmd=="add"){
        DoAdd(param);
        return true;
    }

    if(cmd=="exc"){
        DoExc(param);
        return true;
    }

    if(cmd=="excall"){
        DoExcAll();
        return true;
    }

    if(cmd=="showd"){
        DoShowD();
        return true;
    }

    if(cmd=="size"){
        DoSize(param);
        return true;
    }

    if(cmd=="frees"){
        DoFrees();
        return true;
    }

    if(cmd=="discon"){
        DoDiscon();
        return true;
    }

    if(cmd=="help"){
        PrintHelp();
        return true;
    }

    cout<<"\tWrong command!"<<endl;
    return false;

}

void FileRecv::Run(){
    cout<<g_dir_exp_c->GetCurrentDir()<<g_dir_exp_c->GetSystemSlash();
    std::getline(cin,g_command);

    while(g_command!="exit"){


        Do(GetCommand(g_command),GetParam(g_command));

        if(g_status==CLIENT)
            cout<<g_dir_exp_c->GetCurrentDir()<<g_dir_exp_c->GetSystemSlash();//write current directory
        if(g_status==CCLIENT)
            cout<<g_current_server_dir<<g_dir_exp_c->GetSystemSlash();//write current server location

        cin.clear();
        cin.sync();
        std::getline(cin,g_command);
    }
}

