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
      bool registerClient(const string &name);

      void handleModuleChange(const string& state,const string& incomNum, const string& incomState ); //&mobileClient param/ lambda the same as registerOperData
      /**
 * @brief handle oper data    
 * @param name - set user name 
 */
      void handleOperData(string &name);

      void handleRpc();

      void handleNotification();

      void makeCall(const string &number); //change state
      void setName(const string &name);
      void answer();
      void callEnd();
      void reject();
      void unregister();
      string getNumber();
      void setNumber(const string &number);

   private:
      void setIncomigNumber(const string &number);
      void setState(const string &number, const string &status);

      shared_ptr<ns_NetConf::NetConfAgent> _netConfAgent;
      string _name;
      string _number;
   };

}