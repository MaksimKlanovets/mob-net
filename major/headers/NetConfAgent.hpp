#include "libsysrepocpp/headers/Session.hpp"

using namespace std;



namespace nsMobileClient
{
        class MobileClient;
}

namespace ns_NetConf
{
        class NetConfAgent
        {
        public:
                virtual ~NetConfAgent();

                /** 
* @brief initialization Sysrepo. 
*
*@return true if ok, otherwise false
*/
                virtual bool initSysrepo();
                /** 
* @brief fetching data from  a given path
*
*@param name xpath - path for fetching. 
*
*@param name mapFromFetch - here will save data from a given path. 
            first- key/path. second -value;
*
*@return true if ok, otherwise false
*/
                virtual bool fetchData(const string &xpath, map<string, string> &mapFromFetch);
                /** 
* @brief  Subscribe for model Changes
*
*@param name Module name for subscribe
*
*@return true if ok, otherwise false
*/
                virtual bool subscriberForModelChanges(const string &module_name,
                                                       nsMobileClient::MobileClient &mobClient, const string &xpath);
                /** 
* @brief set noconfig data - username;
*
*@param name module_name - name of module.
*@param name setData - first - path, second value 
*
*@return true if ok, otherwise false
*/
                virtual bool registerOperData(const string &module_name, const string &xpath,
                                              nsMobileClient::MobileClient &mobClient);
                /** 
* @brief subscribe For Rpc/ call back for rpc message
*
*@param name Module name.
*
*@return true if ok, otherwise false
*/
                virtual bool subscriberForRpc(const string &module_name);
                /** 
* @brief notifySysrepo - notifications Sysrepo
*
*@param name Module name.
*
*@return true if ok, otherwise false
*/
                virtual bool notifySysrepo(const string &module_name);
                /** 
* @brief changeData - change data 
*
*@param name setData - set data. First arg path/second value
*
*@return true if ok, otherwise false
*/
                virtual bool changeData(const pair<string, string> &setData);
                /** 
* @brief delete item 
*
*@param path path to delete.
*
*@return true if ok, otherwise false
*/
                virtual bool deleteItem(const string &path);

        private:
                shared_ptr<sysrepo::Session> _session;
                shared_ptr<sysrepo::Subscribe> _subscribe;
                shared_ptr<sysrepo::Connection> _connection;
        };
}
