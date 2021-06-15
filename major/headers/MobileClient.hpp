 #include "NetConfAgent.hpp"
 #include "libsysrepocpp/headers/Session.hpp"
 using namespace std;
 
 namespace nsMobileClient
 {

class MobileClient
 {
 
 public:
     MobileClient();
     ~MobileClient();

/** 
* @brief  register client,init sysrepo/ set noconfig data-User name
*
*@param  module_nane 
*@param  setData  first-path without leaf/ second - name
*
*@return true if ok, otherwise false
*/
bool registerClient(const string &module_name, const pair<string,string> &setData);

void handleModuleChange();
void handleOperData();
void handleRpc();
void handleNotification();

private:
    shared_ptr <ns_NetConf::NetConfAgent> _netConfAgent; 
    string _name;
  
 };

 }
 
 