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
* @brief  
*
*@param  
*@param  
*
*@return true if ok, otherwise false
*/
bool registerClient( const string &name, const string &number);
/**
 * @brief 
 * @param 
 * @return
 */
void handleModuleChange();
/**
 * @brief 
 * @param 
 * @return
 */
void handleOperData(string &name);

void handleRpc();

void handleNotification();



private:
    shared_ptr <ns_NetConf::NetConfAgent> _netConfAgent; 
    string _name;
    string _number;
  
 };

 }
 
 