#ifndef _NET_CONF_AGENT_HPP
#define _NET_CONF_AGENT_HPP
#include "NetConfAgent.hpp"
#endif
#include "libsysrepocpp/headers/Session.hpp"

 namespace ns_NetConf
{
     class NetConfAgent;
}

namespace nsMobileClient
{
     /** 
* @brief class MobileClient main methods  for work with mobile client
*/
     class MobileClient
     {
     public:
          MobileClient();
          MobileClient(unique_ptr<ns_NetConf::NetConfAgent> netConfAgent);
          /**
 * @brief registration user
 * 
 * @param name - user name
 * 
 * @return true if ok, otherwise false
 */
          bool registerClient(const string &name, const string &number);
          /**
 * @brief print a message about event
 * 
 * @param name state caller 
 * *
 * @param incomNum  incoming number
 * 
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
 * 
 * @param 
 * 
 * @return 
 */
          // void handleRpc();
          /**
 * @brief 
 * 
 * @param
 * 
 * @return 
 */
          //void handleNotification();
          /**
 * @brief make call 
 * 
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
 *
 * @return true if ok, otherwise false
 */
          bool unregister();
          /**
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
          /**
 * @brief set outgoing number
 * 
 * @param number - set this number
 */
          void setOutNUm(const string &outNum);
          /**
 * @brief get name
 */
          string getName() const;
          /**
 * @brief create path    
 * 
 * @param key - number, suffix - state
 * 
 * @return all the path 
 */
          string createPath(const string &key, const string &suffix = "");
          /**
 * @brief set registration status
 * 
 * @param setStatus - status registration
 */
          void setIsReg(const bool &setStatus);
          /**
 * @brief get status registration
 * 
 * @return  status registration
 */
          bool getIsReg() const;
          void setState(const string &number, const string &status);
                 /**
 * @brief set whom we are calling, incoming number
 * 
 * @param number  number whom we are calling
 */
          void setIncomigNumber(const string &number);

     private:
   
          /**
 * @brief set state
 * 
 * @param number - key for path 
 * 
 * @param status - state for set
 */
          unique_ptr<ns_NetConf::NetConfAgent> _netConfAgent;
          string _name;
          string _number;
          string _outNum;
          bool _isReg;
     };
}
