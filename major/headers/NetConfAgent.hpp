#include <iostream>

#include "libsysrepocpp/headers/Session.hpp"

using namespace std;
#define MAX_LEN 100

class NetConfAgent
{
private:
    unique_ptr<sysrepo::Session> *m_Session;
    unique_ptr<sysrepo::Subscribe> *m_Subscribe;
    unique_ptr<sysrepo::Connection> *m_Connection;
    
public:
    NetConfAgent();
    ~NetConfAgent();
    //initialization sysrepo
    bool initSysrepo();
    bool closeSysrepo();
    bool fetchData();
    bool subscriberForModelChanges();
    bool registerOperData();
    bool subscriberForRpc();
    bool notifySysrepo();
    bool changeData();
    /* Helper function for printing events. */
    const char *ev_to_str(sr_event_t ev);

};


