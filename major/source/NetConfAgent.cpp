#include "NetConfAgent.hpp"

 #include <signal.h>
 #include <unistd.h>
//what is it ?
volatile int exit_application = 0;

static void
sigint_handler(int signum)
{
    exit_application = 1;
}

NetConfAgent::NetConfAgent()
{
  m_Session = {};
  m_Connection = {};
  m_Subscribe = {};
  
}

NetConfAgent::~NetConfAgent()
{
}

bool NetConfAgent::initSysrepo(){

/* connect to sysrepo */
 cout << "connecting to sysrepo" <<endl;
m_Connection = make_shared<sysrepo::Connection>();


/* start session */
cout << "starting session" << endl;
m_Session = make_shared<sysrepo::Session>(m_Connection);
return true;
}

bool  NetConfAgent::closeSysrepo(){

    /* loop until ctrl-c is pressed / SIGINT is received */
         signal(SIGINT, sigint_handler);
        while (!exit_application) {
            sleep(1000);  /* or do some more useful work... */
        }
        
        cout << "Application exit requested, exiting." << endl;

    m_Session->session_stop();
        
        return true;
}

bool NetConfAgent::fetchData(const char *module_name){
/* read running config */
        cout << "\n\n ========== READING RUNNING CONFIG: ==========\n" << endl;

       char select_xpath[MAX_LEN];
    try {
        snprintf(select_xpath, MAX_LEN, "/%s:*//*", module_name);

        auto values = m_Session->get_items(&select_xpath[0]);
        if (values == nullptr)
            return false;

        for(unsigned int i = 0; i < values->val_cnt(); i++)
            cout << values->val(i)->to_string();
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
    }
    return true;
}



/* Helper function for printing events. */
const char *ev_to_str(sr_event_t ev) {
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

bool NetConfAgent::subscriberForModelChanges(const char *module_name){
/* subscribe for changes in running config */
try
{
     m_Subscribe = make_shared<sysrepo::Subscribe>(m_Session);

        auto cb = [] (sysrepo::S_Session m_Session, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) {
            char change_path[MAX_LEN];

            try {
                cout << "\n\n ========== Notification " << ev_to_str(event) << " =============================================";
                if (SR_EV_CHANGE == event) {
                    cout << "\n\n ========== CONFIG HAS CHANGED, CURRENT RUNNING CONFIG: ==========\n" << endl;
                    print_current_config(m_Session, module_name);
                }

                cout << "\n\n ========== CHANGES: =============================================\n" << endl;

                snprintf(change_path, MAX_LEN, "/%s:*//.", module_name);
                //create data
                auto it = m_Session->get_changes_iter(change_path);

                while (auto change = m_Session->get_change_next(it)) {
                    print_change(change);
                }
                ///////////////
                cout << "\n\n ========== END OF CHANGES =======================================\n" << endl;

            } catch( const std::exception& e ) {
                cout << e.what() << endl;
            }
            return SR_ERR_OK;
            

        };   
      
       m_Subscribe->module_change_subscribe(module_name, cb);

        
}
catch( const std::exception& e ) {
        cout << e.what() << endl;
        return -1;
    }

       
}
bool NetConfAgent::registerOperData(){


}
bool subscriberForRpc(const char *module_name){

}

bool NetConfAgent::notifySysrepo(){

}

bool NetConfAgent::changeData(){

}

void
NetConfAgent::print_current_config(sysrepo::S_Session session, const char *module_name)
{
    char select_xpath[MAX_LEN];
    try {
        snprintf(select_xpath, MAX_LEN, "/%s:*//*", module_name);

        auto values = session->get_items(&select_xpath[0]);
        if (values == nullptr)
            return;

        for(unsigned int i = 0; i < values->val_cnt(); i++)
            cout << values->val(i)->to_string();
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
    }
}

void
NetConfAgent::print_change(sysrepo::S_Change change) {
    cout << endl;
    switch(change->oper()) {
    case SR_OP_CREATED:
        if (nullptr != change->new_val()) {
           cout <<"CREATED: ";
           cout << change->new_val()->to_string();
        }
        break;
    case SR_OP_DELETED:
        if (nullptr != change->old_val()) {
           cout << "DELETED: ";
           cout << change->old_val()->to_string();
        }
    break;
    case SR_OP_MODIFIED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MODIFIED: ";
           cout << "old value ";
           cout << change->old_val()->to_string();
           cout << "new value ";
           cout << change->new_val()->to_string();
        }
    break;
    case SR_OP_MOVED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " after ";
           cout << change->old_val()->xpath();
        } else if (nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " first";
        }
    break;
    }
}




