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
 * 
 * @param name - user name
 * 
 * @return true if ok, otherwise false
 */
          bool registerClient(const string &name);
          /**
 * @brief print a message about event
 * 
 * @param name state caller 
 * @param incomNum  incoming number,
 * @param incomState incoming state
 */
          void handleModuleChange(const string &state, const string &incomNum, const string &incomState);
          /**
 * @brief handle oper data set user name
 * 
 * @param name - user name 
 */
          void handleOperData(string &name);
          /**
 * @brief 
 * @param 
 * @return 
 */
          void handleRpc();
          /**
 * @brief 
 * @param
 * @return 
 */
          void handleNotification();
          /**
 * @brief make call 
 * @param number whom to call
 */
          void makeCall(const string &number);
          /**
 * @brief set private name
 * 
 * @param name for set
 */
          void setName(const string &name);
          /**
 * @brief set state active
 */
          void answer();
          /**
 * @brief set base  params
 */
          void callEnd();
          /**
 * @brief set base  param, when is calling but not answer/it's reject
 */
          void reject();
          /**
 *@brief delelte registration
 */
          void unregister();
          /**
 * @brief getting private number
 */
          string getNumber();
          /**
 * @brief set private number
 * 
 * @param number for set 
 */
          void setNumber(const string &number);
          void setOutNUm(const string &outNum);
          string getName();
     private:
          /**
 * @brief set whom we are calling, incoming number
 * 
 * @param number  number whom we are calling
 */
          bool setIncomigNumber(const string &number);
          /**
 * @brief set state
 * 
 * @param number - key for path 
 * @param status - state for set
 */
          void setState(const string &number, const string &status);

          shared_ptr<ns_NetConf::NetConfAgent> _netConfAgent;
          string _name;
          string _number;
          string _outNum;
     };

}