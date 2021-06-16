#pragma once

 #include "NetConfAgent.hpp"
 #include "libsysrepocpp/headers/Session.hpp"
 using namespace std;
 
namespace ns_NetConf
{
   class NetConfAgent; 
}

 namespace nsMobileClient
 {

class MobileClient
 {
 
 public:
     MobileClient();
     ~MobileClient();

/**
 * @brief registration user
 * @param name - user name. number user number
 * @return true if ok, otherwise false
 */
bool registerClient( const string &name, const string &number);

void handleModuleChange();//&mobileClient param/ lambda the same as registerOperData
/**
 * @brief handle oper data    
 * @param name - set user name 
 */
void handleOperData(string &name);

void handleRpc();

void handleNotification();

bool setState(const string &number);
void  setName(const string &name);
void setNumber(const string &number);
bool makeCall(const string &number);//change state

private:
    shared_ptr <ns_NetConf::NetConfAgent> _netConfAgent; 
    string _name;
    string _number;
  
 };

 }
 
 