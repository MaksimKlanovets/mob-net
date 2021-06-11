
#include "libsysrepocpp/headers/Session.hpp"

using namespace std;

#define MAX_LEN 100

class NetConfAgent
{
private:
    shared_ptr<sysrepo::Session> m_Session;
    shared_ptr<sysrepo::Connection> m_Connection;
public:
    NetConfAgent();
    ~NetConfAgent();

    //initialization sysrepo
    bool initSysrepo( );
    //write data from path
    bool fetchData(const char *xpath,libyang::S_Data_Node *data,const string &key);
    //subcriber for model Changes 
    bool subscriberForModelChanges(const string *module_name);
    // set noconfig data, call back for them 
    bool registerOperData( const string *module_name,const string *xpath, map<string,string>*userName);

    bool subscriberForRpc(const string *module_name);
    //sent some notif 
    bool notifySysrepo(const string *module_name);
    //change data for a given path
    bool changeData(const char *xpath,const string &value);

      
    /* Function to print current configuration state.
 * It does so by loading all the items of a session and printing them out. */
void print_current_config(sysrepo::S_Session session, const char *module_name);
// Helper function for printing changes given operation, old and new value. 
void print_change(sysrepo::S_Change change);
//helper function for print nota
void print_node(libyang::S_Data_Node &node);
//helper function for print type 
const char * nodetype2str(LYS_NODE type);
static
void print_value(sysrepo::S_Val value);
};


