#include "NetConfAgent.hpp"

 #include <signal.h>
 #include <unistd.h>

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
    try
    {
        /* connect to sysrepo */
        cout << "connecting to sysrepo" <<endl;
        m_Connection = make_shared<sysrepo::Connection>();

        /* start session */
        cout << "starting session" << endl;
        m_Session = make_shared<sysrepo::Session>(m_Connection);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

bool NetConfAgent::fetchData(const char *xpath,libyang::S_Data_Node *data, const string &key)
{
     cout << "called fetchData" << endl;

    /* read running config */
    if (m_Session->get_subtree(xpath))
    {
        *data = m_Session->get_subtree(xpath);
        print_node(*data);
        return true;
    }
    cout << "noda is empty " <<endl;

    return false;
}

bool NetConfAgent::subscriberForModelChanges(const string *module_name)
{
/* subscribe for changes in running config */
    auto m_Subscribe=  make_shared<sysrepo::Subscribe>(m_Session);

    auto cb = [] (sysrepo::S_Session m_Session, const char *module_name, const char *xpath, sr_event_t event,
        uint32_t request_id) 
        {
            cout<< "called subscribers for model changes cb" << endl;
            return SR_ERR_OK;
        };   
      
    m_Subscribe->module_change_subscribe(module_name->c_str(), cb);

     /* read running config */
    cout << "\n\n ========== READING RUNNING CONFIG: ==========\n" << endl;
    print_current_config(m_Session, module_name->c_str());
    
}


bool NetConfAgent::registerOperData( const string *module_name, const string *xpath,map<string,string>*userName)
{
    map<string,string>::iterator it = userName->find(*xpath);
    cout << "Application will provide data of " << *module_name << endl;
    auto subscribe = std::make_shared<sysrepo::Subscribe>(m_Session);
        
    auto cb = [it] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
        uint32_t request_id, libyang::S_Data_Node &parent) 
    {
        cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;
        //it's index of leef for new noconfig data
        string pathNewSubTree =it->first+  "/userName";
    
        cout << it->first << it->second << endl;
        libyang::S_Context ctx = session->get_context();
        libyang::S_Module mod = ctx->get_module(module_name);
        parent->new_path(ctx, pathNewSubTree.c_str(), it->second.c_str(),LYD_ANYDATA_CONSTSTRING, 0);
              
        return SR_ERR_OK;
    };

     subscribe->oper_get_items_subscribe(module_name->c_str(), cb, xpath->c_str());

    /* loop until ctrl-c is pressed / SIGINT is received */
    // signal(SIGINT, sigint_handler);
    // while (!exit_application) {
    //     sleep(1000);  /* or do some more useful work... */
    // }
}
bool NetConfAgent::subscriberForRpc(const char *module_name)
{
}

bool NetConfAgent::notifySysrepo(const string *module_name)
{
    try 
    {

         libyang::S_Context ctx = m_Connection->get_context();
         libyang::S_Module mod = ctx->get_module(module_name->c_str());
         auto in_trees = std::make_shared<libyang::Data_Node>(ctx, "/mobile-network:test-notif", nullptr, LYD_ANYDATA_CONSTSTRING, 0);
         std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "val1", "some-value"));
         std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "val2", "some-other-value"));

         cout << "\n ========== START NOTIF TREE SEND ==========\n" << endl;
        m_Session->event_notif_send(in_trees);

        cout << "\n ========== END PROGRAM NOTIF==========\n" << endl;
    } 
    catch( const std::exception& e )
    {
        cout << e.what() << endl;
        return -1;
    }
}

bool NetConfAgent::changeData(const char *xpath,const string &value)
{
    cout << "called changeData" <<endl;
    m_Session->set_item_str(xpath,value.c_str());
    
    return true;
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
NetConfAgent::print_change(sysrepo::S_Change change) 
{
    cout << endl;
    switch(change->oper()) 
    {
    case SR_OP_CREATED:
        if (nullptr != change->new_val()) 
        {
           cout <<"CREATED: ";
           cout << change->new_val()->to_string();
        }
        break;
    case SR_OP_DELETED:
        if (nullptr != change->old_val()) 
        {
           cout << "DELETED: ";
           cout << change->old_val()->to_string();
        }
    break;
    case SR_OP_MODIFIED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) 
        {
           cout << "MODIFIED: ";
           cout << "old value ";
           cout << change->old_val()->to_string();
           cout << "new value ";
           cout << change->new_val()->to_string();
        }
    break;
    case SR_OP_MOVED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) 
        {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " after ";
           cout << change->old_val()->xpath();
        } else if (nullptr != change->new_val()) 
        {
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
    switch (schema->nodetype()) 
    {
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
        for (libyang::S_Schema_Node_Leaf &key : slist.keys()) 
        {
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
    switch (type) 
    {
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

void NetConfAgent::print_value(sysrepo::S_Val value)
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
