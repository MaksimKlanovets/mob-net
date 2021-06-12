
#include "libsysrepocpp/headers/Session.hpp"
using namespace std;
//data for test function NetConfAgent::print_current_config
#define MAX_LEN 100


//tasks 1. unown namespace in .cpp for helper function and const, difane
//futch data with mape path and value
// change point to refer
// _subcriber move to private,  private fild start with _
//registerOperdata move xpath to first arg in map
//think about implementation thread

namespace ns_NetConf
{

class NetConfAgent
{
public:
    NetConfAgent();
    ~NetConfAgent();
/*
*@breaf Fethch data....
*
*@param name Module name.
*
*@return true if ok, otherwise false
*/
    //initialization sysrepo
    bool initSysrepo( );
    //write data from path
    bool fetchData(pair<string,string>&pairFromFetch);
    //subcriber for model Changes 
    bool subscriberForModelChanges(const string &module_name);
    // set noconfig data, call back for them 
    bool registerOperData( const string &module_name,const pair<string,string> &setData);

    bool subscriberForRpc(const string &module_name);
    //sent some notif 
    bool notifySysrepo(const string &module_name);
    //change data for a given path
    bool changeData(const pair<string,string> &setData);

      
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

private:
//rename first _session, add _subscriber in private field
    shared_ptr<sysrepo::Session> _session;
    shared_ptr<sysrepo::Subscribe> _subscribe;
    shared_ptr<sysrepo::Connection> _connection;
};

}







