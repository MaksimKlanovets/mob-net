#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <string>
#include "NetConfAgent.hpp"
#include <cstring>

#include "libsysrepocpp/headers/Session.hpp"

using namespace std;


//erase temp comands
void resetData(string &str1,string &str2);
//pring list of commmands
void listCommands(const vector<string> &com);
//if ch = 0 must arg if ch=1 than cannot arg
void printErr(const string &com, int ch = 0);
//separate arg from comman, return if arg exist
bool separateComArg(string &com,string &arg);
//command must contain an arg
bool cMustArg(const string &com, const string &arg); 
//command hasn't to contain an arg
bool cNoArg(const string &com, const string &arg);

int main(int argc, char **argv)
{
    //////////////////////////////////////////////////////////////
const char *module_name = "mobile-network";

const char *xpath = "/mobile-network:core/subscribers";
///////////////test data/////////////////
string key = "001";
string status = "busy"; 

libyang::S_Data_Node data = {};
////////////////////////////////////////

NetConfAgent netConfAgent;

netConfAgent.initSysrepo();
netConfAgent.subscriberForModelChanges(module_name);
netConfAgent.fetchData(xpath,&data,key);
netConfAgent.changeData(xpath, &data,status,key);
netConfAgent.fetchData(xpath,&data,key);

netConfAgent.subscriberForRpc(module_name);

int stop;
cin >>stop;

///////////////////////////////////////////////////////////////////////////////////////////////////
string tempComand {};
string tempArg{};
bool isReg = false;

//commands with arg register,call,name
const vector<string> m_com{"register","unregister","call","name",
                        "answer","reject","callEnd","help","exit"};

map<string,function<void()>> my_map;
 
my_map.emplace("help", [&m_com]() { listCommands(m_com); });
my_map.emplace("register",[&tempComand,&tempArg,&isReg](){
    if (cMustArg(tempComand,tempArg) && !isReg)
    { 
       isReg = true;
    }
    else if (isReg){
    cout << "Only one registration" << endl;
    }
});
my_map.emplace("unregister",[&tempComand,&tempArg,&isReg](){
    cNoArg(tempComand,tempArg);
    isReg = false;
});
my_map.emplace("call", [&tempComand,&tempArg]() {
    cMustArg(tempComand,tempArg);
 });
my_map.emplace("name", [&tempComand,&tempArg]() {
    cMustArg(tempComand,tempArg);
 });
my_map.emplace("answer", [&tempComand,&tempArg]() { 
    cNoArg(tempComand,tempArg);
});
my_map.emplace("reject", [&tempComand,&tempArg]() { 
    cNoArg(tempComand,tempArg);
});
my_map.emplace("callEnd", [&tempComand,&tempArg]() { 
    cNoArg(tempComand,tempArg);
});
my_map.emplace("exit", []() { 
    //can call isReg
cout <<"Exit" <<endl;
return 0;
});

// while (true)
// {
//     resetData(tempComand,tempArg);
//     getline(cin,tempComand);
//     separateComArg(tempComand,tempArg);
//     map<string,function<void()>>::iterator it = my_map.find(tempComand);
//     if(it != my_map.end())
//     {
// it->second();
//     }
    
// }

  return 0;
}

void resetData(string &str1,string &str2)
{
    str1.clear();
    str2.clear();
}

void listCommands(const vector<string> &com){
    for(const auto &elem : com ) {
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
bool separateComArg(string &com,string &arg){
 if (com.find(' ') != string::npos) {
     arg = com.substr(com.find(' ')+1,com.size() -1);
     com.erase(com.find(' '),com.size() -1);
     return true;
    }
    return false;
}
bool cMustArg(const string &com, const string &arg) {
if (!arg.empty()) {
    cout << "command-> " << com << "  Arg-> " << arg << endl;
    return true;
}
printErr(com);
return false;
}
bool cNoArg(const string &com, const string &arg){
    if (arg.empty())
    {
        cout << "command-> " << com  << endl;
        return true;
    }
    printErr(com,1);
    return false;
}