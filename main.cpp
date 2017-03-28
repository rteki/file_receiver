#include "filerecv.h"
using namespace std;

int main()
{
    setlocale(LC_CTYPE,"ukr");
    cout<<"\tFile Receiver v1.0\n\tAuthor: Bohdan Shevchenko\n\tEmail: bohdan.u.shevchenko@gmail.com\n\n";
    cout<<"\tPrint \"help\" for instructions.\n\n";
    FileRecv app;
    app.Run();

    return 0;
}


