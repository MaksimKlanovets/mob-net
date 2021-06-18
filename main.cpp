#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <string>
#include "MobileClient.hpp"
#include <cstring>

#include "libsysrepocpp/headers/Session.hpp"

using namespace std;

namespace 
{
    //erase temp comands
    void resetData(string &str1,string &str2)
    {
     str1.clear();
     str2.clear();
    }
    //pring list of commmands
    void listCommands(const vector<string> &com)
    {
        for(const auto &elem : com ) 
        {
            cout<< elem <<endl;
        }
    }
    //if ch = 0 function must have an arg, if ch=1 than cannot arg
    void printErr(const string &com, int ch = 0 )
    {
        if(ch == 0) 
        {
            cout << com << " must contains an argument" << endl;
        }
        if (ch == 1)    
        {
            cout << com << " cannot contains an argument" << endl;
        }
    }
    //separate arg from comman
    bool separateComArg(string &com,string &arg)
    {
        if (com.find(' ') != string::npos) 
        {
            arg = com.substr(com.find(' ')+1,com.size() -1);
            com.erase(com.find(' '),com.size() -1);
            return true;
        }
        return false;
    }
    //command must contain an arg/return true if ok
    bool cMustArg(const string &com, const string &arg) 
    {
        if (!arg.empty()) 
        {
           // cout << "command-> " << com << "  Arg-> " << arg << endl;
            return true;
        }
        printErr(com);
        return false;
    }
    //command hasn't to contain an arg
    bool cNoArg(const string &com, const string &arg)
    {
        if (arg.empty())
        {
            //cout << "command-> " << com  << endl;
            return true;
        }
        printErr(com,1);
        return false;
    }
} 

int main(int argc, char **argv)
{

const string module_name = "mobile-network";
string tempComand {};
string tempArg{};
bool isReg = false;

//commands with arg register,call,name
const vector<string> m_com
{
    "register [name] - User registration ",
    "unregister - delete registration ",
    "callEnd - end the incoming call ",
    "reject - cancel incoming call ",
    "call [phoneNumber] - call to ",
    "name [name] - change name ", 
    "answer - take the call ",
    "help",
    "exit"
};

map<string,function<void()>> my_map;
 
nsMobileClient::MobileClient mobileClient;

string errorL{};

my_map.emplace("help", [&]() 
{ 
    listCommands(m_com); 
});
//
my_map.emplace("register",[&]()
{
    if (cMustArg(tempComand,tempArg) && !isReg)
    { 
        string number;
        cout << "input your phone number" <<endl;
        cin >>number;
        if (!mobileClient.registerClient(tempArg))
        {
            errorL =  "register is done with error";
            return ;
        }
        
        mobileClient.setNumber(number);
      
        isReg = true;

    }
    else if (isReg)
    {
        cout << "Only one registration" << endl;
    }
});
my_map.emplace("unregister",[&]()
{
    if (cNoArg(tempComand,tempArg) && isReg)
    {
        mobileClient.unregister();
        cout << "registration was deleted" << endl;
    }
    
    isReg = false;
});
my_map.emplace("call", [&]() 
{
    if (cMustArg(tempComand,tempArg) && isReg)
    {
        mobileClient.makeCall(tempArg);
    }
    
});
my_map.emplace("name", [&]() 
{
    if (cMustArg(tempComand,tempArg) && isReg)
    {
        mobileClient.setName(tempArg);
    }
});
my_map.emplace("answer", [&]() 
{ 
    if (cNoArg(tempComand,tempArg) && isReg)
    {
        mobileClient.answer();
    }
});
my_map.emplace("reject", [&]() 
{ 
     if (cNoArg(tempComand,tempArg) && isReg)
    {
        mobileClient.reject();
    }
});
my_map.emplace("callEnd", [&]() 
{ 
     if (cNoArg(tempComand,tempArg) && isReg)
    {
        mobileClient.callEnd();
    }
});
my_map.emplace("exit", []() 
{ 
    //can call isReg
    cout <<"Exit" <<endl;
    
});
cout << "Program is working. input help for list commands" <<endl;
while (true)
{
    resetData(tempComand,tempArg);
    getline(cin,tempComand);
    separateComArg(tempComand,tempArg);
    map<string,function<void()>>::iterator it = my_map.find(tempComand);
    if(it != my_map.end())
    {
        if (it->first != "exit")
        {

        it->second(); 

        if (!errorL.empty())
        {
            cout << errorL << endl;
            errorL.clear();
        }
        
           continue;
        }
        break;
    }
}

  return 0;
}

