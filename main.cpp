#include <iostream>

using namespace std;

struct COMAND
{
   const string c_register   = "register";//arg
   const string c_unregister = "unregister";
   const string c_call       = "call";//arg
   const string c_name       = "name";//arg
   const string c_answer     = "answer";
   const string c_reject     = "reject";
   const string c_callEnd    = "callEnd";
   const string c_help       = "help";
   const string c_exit       = "exit";    
};

//erase temp comands
void resetData(string &str1,string &str2);
void resetData(string &str1);

//pring list of commmands
void listCommands();
//if ch = 0 must arg if ch=1 than cannot arg
void printErr(const string &com, int ch = 0);

int main()
{
COMAND menu;
while(true){    
    string tempComand{};
    string tempArg{};  

    getline(cin,tempComand);

    //separate an argumen if its exist
    if (tempComand.find(' ') != string::npos) {
     tempArg = tempComand.substr(tempComand.find(' ')+1,tempComand.size() -1);
     tempComand.erase(tempComand.find(' '),tempComand.size() -1);
    }
    
    if(tempComand == menu.c_help)    {
        listCommands();
        resetData(tempComand,tempArg);
        continue;
    }

    if (tempComand == menu.c_register ) {
        if(tempArg.empty()) {
            printErr(tempComand);
            resetData(tempComand);
            continue;
        }
        cout << "command-> " << tempComand << "  Arg-> " << tempArg << endl;
    }

    else if (tempComand == menu.c_unregister )  {
        if (!tempArg.empty())   {
            printErr(tempComand,1);
            resetData(tempComand,tempArg);
            continue;
        }
        cout << "command unregister" <<endl;
    }

    else if (tempComand == menu.c_call )    {
        if (tempArg.empty())    {
            printErr(tempComand);
            resetData(tempComand);
            continue;
        }            
        cout << "command " <<tempComand << "  Arg -> " << tempArg << endl;
    }

    else if (tempComand == menu.c_name )    {
        if (tempArg.empty())    {
            printErr(tempComand);
            resetData(tempComand);
            continue;
        }
        cout << "comman " << tempComand << "  Arg -> " << tempArg << endl;
    }

    else if (tempComand == menu.c_answer )  {
        if (!tempArg.empty())   {
            printErr(tempComand,1);
            resetData(tempComand,tempArg);
            continue;
        }
        cout << "command answer" <<endl;
    }

    else if (tempComand == menu.c_reject )  {
        if (!tempArg.empty())   {
            printErr(tempComand,1);
            resetData(tempComand,tempArg);
            continue;
        }
        cout << "command reject" <<endl;
    }

    else if (tempComand == menu.c_callEnd ) {
        if (!tempArg.empty())   {
            printErr(tempComand,1);
            resetData(tempComand,tempArg);
            continue;
        }
        cout << "comman call end" <<endl;
    }

    else if (tempComand == menu.c_exit )    {
        if (!tempArg.empty())   {
            printErr(tempComand,1);
            resetData(tempComand,tempArg);
            continue;
        }
        cout << "command exit" <<endl;
        return 0;
    }

    else {
        cout<< "this comman does not exist" << endl;
        resetData(tempComand,tempArg);
    }
      
 }
  return 0;
}

void resetData(string &str1,string &str2)
{
    str1.clear();
    str2.clear();
}
void resetData(string &str1)
{
     str1.clear();
}
void listCommands(){
   const string com[] = {"register","unregister","call","name","answer","reject",
                    "callEnd","help","exit"};
                    for(const auto &elem : com )
                    {
                        cout<< elem <<endl;
                    }
}

void printErr(const string &com, int ch ){
    if(ch == 0) {
        cout << com << " must contains an argument" << endl;
    }
    if (ch == 1)    {
        cout << com << " cannot contains an argument" << endl;
    }
}