#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <string>
//#include "NetConfAgent.hpp"
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
    //command must contain an arg
    bool cMustArg(const string &com, const string &arg) 
    {
        if (!arg.empty()) 
        {
            cout << "command-> " << com << "  Arg-> " << arg << endl;
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
            cout << "command-> " << com  << endl;
            return true;
        }
        printErr(com,1);
        return false;
    }
} 

int main(int argc, char **argv)
{


/////////////////END TEST DATA netconfAgent//////////////////////////////////////////////////////////////////////////////////
// string xpathOperdat= "/mobile-network:core/subscribers[number = '001']";
// pair<string, string> userPrivateData = make_pair(xpathOperdat,"this name");
///////////////////////
// string pathFoFetchData = "/mobile-network:core/subscribers[number = '001']";
// map<string,string>mapFromFetch;
//////////////////////
//pair<string,string> dataForChange = make_pair("/mobile-network:core/subscribers[number = '001']/state","busy");
// ns_NetConf::NetConfAgent netConfAgent;
// netConfAgent.initSysrepo();
//netConfAgent.subscriberForModelChanges(module_name);
//netConfAgent.registerOperData(module_name,userPrivateData);
//netConfAgent.changeData(dataForChange);
// netConfAgent.fetchData(pathFoFetchData,mapFromFetch);
// for (map<string,string>::const_iterator it = mapFromFetch.begin(); it != mapFromFetch.end(); it++)
// {
//     cout << "key->  " << it->first << "\n" << "value->  " << it->second <<endl;
// }
//netConfAgent.notifySysrepo(module_name);
//netConfAgent.subscriberForRpc(module_name);

//  int pause1;
//  cin >>pause1;
///////////////END TEST DATA NETCONFAGENT///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string module_name = "mobile-network";
string tempComand {};
string tempArg{};
bool isReg = false;

//commands with arg register,call,name
const vector<string> m_com
{
    "register [name]- User registration ","unregister - delete registration",
    "call [phoneNumber]- call to  ","name - change name or set ", "answer - take the call",
    "reject - is talking to ","callEnd - end the call","help","exit"
};

map<string,function<void()>> my_map;
 



nsMobileClient::MobileClient mobileClient;

my_map.emplace("help", [&m_com]() 
{ 
    listCommands(m_com); 
});
my_map.emplace("register",[&tempComand,&tempArg,&isReg,&mobileClient]()
{
    if (cMustArg(tempComand,tempArg) && !isReg)
    { 
        string number;// = ;
        cout << "input your phone number" <<endl;
        cin >>number;
        mobileClient.registerClient(tempArg, number);
       
       isReg = true;

    }
    else if (isReg)
    {
        cout << "Only one registration" << endl;
    }
});
my_map.emplace("unregister",[&tempComand,&tempArg,&isReg,&mobileClient]()
{
    if (cNoArg(tempComand,tempArg) && isReg)
    {
        mobileClient.~MobileClient();
        cout << "registration was deleted" << endl;
    }
    
    
    isReg = false;
});
my_map.emplace("call", [&tempComand,&tempArg]() 
{
    cMustArg(tempComand,tempArg);
});
my_map.emplace("name", [&tempComand,&tempArg]() 
{
    cMustArg(tempComand,tempArg);
});
my_map.emplace("answer", [&tempComand,&tempArg]() 
{ 
    cNoArg(tempComand,tempArg);
});
my_map.emplace("reject", [&tempComand,&tempArg]() 
{ 
    cNoArg(tempComand,tempArg);
});
my_map.emplace("callEnd", [&tempComand,&tempArg]() 
{ 
    cNoArg(tempComand,tempArg);
});
my_map.emplace("exit", []() 
{ 
    //can call isReg
    cout <<"Exit" <<endl;
    return 0;
});




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
           continue;
        }
        break;
    }
    
 }

  return 0;
}

