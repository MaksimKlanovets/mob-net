#include "NetConfAgent.hpp"

NetConfAgent::NetConfAgent()
{
  // m_Session = new unique_ptr<sysrepo::Session>;
  m_Session = nullptr;
  // m_Connection = new unique_ptr<sysrepo::Connection>;
  m_Connection = nullptr;
  // m_Subscribe = new unique_ptr<sysrepo::Subscribe>;
  m_Subscribe = nullptr;
  
}

NetConfAgent::~NetConfAgent()
{
}

bool NetConfAgent::initSysrepo(){

/* connect to sysrepo */
 cout << "connecting to sysrepo" <<endl;
m_Connection = new unique_ptr<sysrepo::Connection>();
/* start session */
cout << "starting session" << endl;
m_Session = new unique_ptr<sysrepo::Session>();

}

bool closeSysrepo(){

}

bool fetchData(){

}

bool NetConfAgent::subscriberForModelChanges(){
/* subscribe for changes in running config */
        m_Subscribe = new unique_ptr<sysrepo::Subscribe>;
        shared_ptr<sysrepo::Session> subscriber(move(*m_Session));

        auto cb = [] (sysrepo::S_Session sess, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) {
            char change_path[MAX_LEN];

            try {
                cout << "\n\n ========== Notification " << ev_to_str(event) << " =============================================";
                if (SR_EV_CHANGE == event) {
                    cout << "\n\n ========== CONFIG HAS CHANGED, CURRENT RUNNING CONFIG: ==========\n" << endl;
                    print_current_config(sess, module_name);
                }

                cout << "\n\n ========== CHANGES: =============================================\n" << endl;

                snprintf(change_path, MAX_LEN, "/%s:*//.", module_name);

                auto it = sess->get_changes_iter(change_path);

                while (auto change = sess->get_change_next(it)) {
                    print_change(change);
                }

                cout << "\n\n ========== END OF CHANGES =======================================\n" << endl;

            } catch( const std::exception& e ) {
                cout << e.what() << endl;
            }
            return SR_ERR_OK;

        };     
      
         
}
bool NetConfAgent::registerOperData(){

}

bool NetConfAgent::subscriberForRpc(){

}

bool NetConfAgent::notifySysrepo(){

}

bool NetConfAgent::changeData(){

}

/* Helper function for printing events. */
const char NetConfAgent::*ev_to_str(sr_event_t ev) {
    switch (ev) {
    case SR_EV_CHANGE:
        return "change";
    case SR_EV_DONE:
        return "done";
    case SR_EV_ABORT:
    default:
        return "abort";
    }
}