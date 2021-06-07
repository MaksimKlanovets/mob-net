#include "NetConfAgent.hpp"


 #include <signal.h>
 #include <unistd.h>
//what is the type?
volatile int exit_application = 0;

static void
sigint_handler(int signum)
{
    exit_application = 1;
}

NetConfAgent::NetConfAgent()
{
}

NetConfAgent::~NetConfAgent()
{
}

bool NetConfAgent::initSysrepo( )
{
    /* connect to sysrepo */
    cout << "connecting to sysrepo" <<endl;
    m_Connection = make_shared<sysrepo::Connection>();

    /* start session */
    cout << "starting session" << endl;
    m_Session = make_shared<sysrepo::Session>(m_Connection);

    return true;
}

bool NetConfAgent::fetchData(const char *xpath,libyang::S_Data_Node *data)
{
/* read running config */
    cout << "called fetchData" << endl;
    try 
    {
        *data = m_Session->get_subtree("/mobile-network:core/subscribers[number='001']/state");
        print_node(*data);
    } 
    catch( const std::exception& e ) 
    {
        cout << e.what() << endl;
    }
    
    return true;
}



/* Helper function for printing events. */
const char *ev_to_str(sr_event_t ev) 
{
    switch (ev) 
    {
        case SR_EV_CHANGE:
            return "change";
        case SR_EV_DONE:
            return "done";
        case SR_EV_ABORT:
            default:
        return "abort";
    }
}

bool NetConfAgent::subscriberForModelChanges(const char *module_name)
{
/* subscribe for changes in running config */
       auto m_Subscribe=  make_shared<sysrepo::Subscribe>(m_Session);

try
{
        auto cb = [] (sysrepo::S_Session m_Session, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) {
            char change_path[MAX_LEN];

            try 
            {
                cout << "\n\n ========== Notification " << ev_to_str(event) << " =============================================";
                if (SR_EV_CHANGE == event) {
                    cout << "\n\n ========== CONFIG HAS CHANGED, CURRENT RUNNING CONFIG: ==========\n" << endl;
                    print_current_config(m_Session, module_name);
                }

                cout << "\n\n ========== CHANGES: =============================================\n" << endl;

                snprintf(change_path, MAX_LEN, "/%s:*//.", module_name);
                //create data
                auto it = m_Session->get_changes_iter(change_path);

                while (auto change = m_Session->get_change_next(it)) 
                {
                    print_change(change);
                }
                ///////////////
                cout << "\n\n ========== END OF CHANGES =======================================\n" << endl;

            } 
            catch( const std::exception& e ) 
            {
                cout << e.what() << endl;
            }
            return SR_ERR_OK;
            

        };   
      
       m_Subscribe->module_change_subscribe(module_name, cb);

        /* read running config */
        cout << "\n\n ========== READING RUNNING CONFIG: ==========\n" << endl;
        print_current_config(m_Session, module_name);
}
catch( const std::exception& e ) 
{
        cout << e.what() << endl;
        return -1;
}

     /* loop until ctrl-c is pressed / SIGINT is received */
         signal(SIGINT, sigint_handler);
while (!exit_application) 
{
    sleep(1000);  /* or do some more useful work... */
}
        
        cout << "Application exit requested, exiting." << endl;  
}


bool NetConfAgent::registerOperData(){

   

}
bool NetConfAgent::subscriberForRpc(const char *module_name){
   
}

bool NetConfAgent::notifySysrepo(){

}

bool NetConfAgent::changeData(const char *module_name,const char *xpath, libyang::S_Data_Node *data){
//set_item
//set_user
//aply_changes
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

void
NetConfAgent::print_node(libyang::S_Data_Node &node)
{
    libyang::S_Schema_Node schema = node->schema();

    cout << nodetype2str(schema->nodetype()) << " \"" << schema->name() << '\"' << endl;
    cout << '\t' << "Path: " << node->path() << endl;
    cout << '\t' << "Default: " << (node->dflt() ? "yes" : "no") << endl;

    /* type-specific print */
    switch (schema->nodetype()) {
    case LYS_CONTAINER:
    {
        libyang::Schema_Node_Container scont(schema);

        cout << '\t' << "Presence: " << (scont.presence() ? "yes" : "no") << endl;
        break;
    }
    case LYS_LEAF:
    {
        libyang::Data_Node_Leaf_List leaf(node);
        libyang::Schema_Node_Leaf sleaf(schema);

        cout << '\t' << "Value: \"" << leaf.value_str() << '\"' << endl;
        cout << '\t' << "Is key: " << (sleaf.is_key() ? "yes" : "no") << endl;
        break;
    }
    case LYS_LEAFLIST:
    {
        libyang::Data_Node_Leaf_List leaflist(node);

        cout << '\t' << "Value: \"" << leaflist.value_str() << '\"' << endl;
        break;
    }
    case LYS_LIST:
    {
        libyang::Schema_Node_List slist(schema);

        cout << '\t' << "Keys:";
        for (libyang::S_Schema_Node_Leaf &key : slist.keys()) {
            cout << ' ' << key->name();
        }
        cout << endl;
        break;
    }
    default:
        break;
    }

    cout << endl;
}

const char *
NetConfAgent::nodetype2str(LYS_NODE type)
{
    switch (type) {
    case LYS_CONTAINER:
        return "container";
    case LYS_LEAF:
        return "leaf";
    case LYS_LEAFLIST:
        return "leaf-list";
    case LYS_LIST:
        return "list";
    case LYS_ANYXML:
        return "anyxml";
    case LYS_NOTIF:
        return "notification";
    case LYS_RPC:
        return "rpc";
    case LYS_ACTION:
        return "action";
    case LYS_ANYDATA:
        return "anydata";
    default:
        break;
    }

    return NULL;
}

void
NetConfAgent::print_value(sysrepo::S_Val value)
{
    cout << value->xpath();
    cout << " ";
    switch (value->type()) {
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
        cout << "(container)" << endl;
        break;
    case SR_LIST_T:
        cout << "(list instance)" << endl;
        break;
    case SR_STRING_T:
        cout << "= " << value->data()->get_string() << endl;;
        break;
    case SR_BOOL_T:
    if (value->data()->get_bool())
            cout << "= true" << endl;
    else
            cout << "= false" << endl;
        break;
    case SR_ENUM_T:
        cout << "= " << value->data()->get_enum() << endl;;
        break;
    case SR_UINT8_T:
        cout << "= " << unsigned(value->data()->get_uint8()) << endl;
        break;
    case SR_UINT16_T:
        cout << "= " << unsigned(value->data()->get_uint16()) << endl;
        break;
    case SR_UINT32_T:
        cout << "= " << unsigned(value->data()->get_uint32()) << endl;
        break;
    case SR_UINT64_T:
        cout << "= " << unsigned(value->data()->get_uint64()) << endl;
        break;
    case SR_INT8_T:
        cout << "= " << value->data()->get_int8() << endl;
        break;
    case SR_INT16_T:
        cout << "= " << value->data()->get_int16() << endl;
        break;
    case SR_INT32_T:
        cout << "= " << value->data()->get_int32() << endl;
        break;
    case SR_INT64_T:
        cout << "= " << value->data()->get_int64() << endl;
        break;
     case SR_IDENTITYREF_T:
        cout << "= " << value->data()->get_identityref() << endl;
        break;
    case SR_BITS_T:
        cout << "= " << value->data()->get_bits() << endl;
        break;
    case SR_BINARY_T:
        cout << "= " << value->data()->get_binary() << endl;
        break;
    default:
        cout << "(unprintable)" << endl;
    }
    return;
}
