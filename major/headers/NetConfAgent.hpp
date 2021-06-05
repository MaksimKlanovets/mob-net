
#include "libsysrepocpp/headers/Session.hpp"

using namespace std;

#define MAX_LEN 100



class NetConfAgent
{
private:
    shared_ptr<sysrepo::Session> m_Session;
    shared_ptr<sysrepo::Subscribe> m_Subscribe;
    shared_ptr<sysrepo::Connection> m_Connection;
    
    
public:
    NetConfAgent();
    ~NetConfAgent();

    friend const char *ev_to_str(sr_event_t ev) ; 
    //initialization sysrepo
    bool initSysrepo();
    bool closeSysrepo();
    // read data get
    bool fetchData();
    bool subscriberForModelChanges(const char &module_name);
    // get noconfig data,cli pointer of function witch set data
    bool registerOperData();
    bool subscriberForRpc();
    bool notifySysrepo();
    bool changeData();

      
    /* Function to print current configuration state.
 * It does so by loading all the items of a session and printing them out. */
static void
print_current_config(sysrepo::S_Session session, const char *module_name);

// Helper function for printing changes given operation, old and new value. 
static void
print_change(sysrepo::S_Change change);

  
};

