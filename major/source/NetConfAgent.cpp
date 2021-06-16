#include "NetConfAgent.hpp"
#include "MobileClient.hpp"
 #include <signal.h>
 #include <unistd.h>
 #include "libsysrepocpp/headers/Session.hpp"
namespace nsMobileClient
 {
     class MobileClient;
 }

 namespace
 {
//data for test function NetConfAgent::print_current_config
#define MAX_LEN 100

/* Helper function for printing events. */
const char *ev_to_str(sr_event_t ev) 
{
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

void print_current_config(sysrepo::S_Session session, const char *module_name)
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

void print_change(sysrepo::S_Change change) 
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

const char *nodetype2str(LYS_NODE type)
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

void print_node(libyang::S_Data_Node &node)
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



void print_value(sysrepo::S_Val value)
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
 }
 
namespace ns_NetConf
{
  
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
        _connection = make_shared<sysrepo::Connection>();
        /* start session */
        cout << "starting session" << endl;
        _session = make_shared<sysrepo::Session>(_connection);
        _subscribe = make_shared<sysrepo::Subscribe>(_session);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

bool NetConfAgent::fetchData(const string & xpath, map<string,string>&mapFromFetch)
{
    //cout << "called fetch data " << endl;
    try
    {
       libyang::S_Data_Node data = _session->get_data(xpath.c_str());

        /* go through all top-level siblings */
        for (libyang::S_Data_Node &root : data->tree_for()) 
        {
            for (libyang::S_Data_Node &node : root->tree_dfs()) 
            {
                 libyang::S_Schema_Node schema = node->schema();

                 switch (schema->nodetype()) 
                {
                    case LYS_LEAF:
                    {
                        libyang::Data_Node_Leaf_List leaf(node);
                        libyang::Schema_Node_Leaf sleaf(schema);
                        mapFromFetch.emplace(make_pair(node->path(),string(leaf.value_str())));
                        break;
                    }
                }

            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
return true;
}

bool NetConfAgent::subscriberForModelChanges(const string &module_name,nsMobileClient::MobileClient &mobClient,
                                            const string &xpath )
{
     
    try
    {
        /* subscribe for changes in running config */
         auto cb = [&mobClient] (sysrepo::S_Session sess, const char *module_name, 
         const char *xpath, sr_event_t event,  uint32_t request_id) 
        {
            mobClient.handleModuleChange();
            return SR_ERR_OK;
        };   

        _subscribe->module_change_subscribe(module_name.c_str(), cb,xpath.c_str());

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}


bool NetConfAgent::registerOperData( const string &module_name, const string &xpath ,
nsMobileClient::MobileClient &mobClient)
{
    cout << "Application will provide data of " << module_name << endl;
     
        try
        {
            auto cb = [&mobClient,xpath] (sysrepo::S_Session session, const char *module_name, 
            const char *path, const char *request_xpath, uint32_t request_id,
            libyang::S_Data_Node &parent) 
            {
                cout << "\n\n ========== registerOperData " << endl;
                string name;
                mobClient.handleOperData(name);

                 libyang::S_Context ctx = session->get_context();
                 libyang::S_Module mod = ctx->get_module(module_name);
                 string pathUserName = xpath + "/userName";
                 
                cout << pathUserName <<endl;
                parent->new_path(ctx,pathUserName.c_str(), name.c_str(),LYD_ANYDATA_CONSTSTRING, 0);
                
                return SR_ERR_OK;
            };
            _subscribe->oper_get_items_subscribe(module_name.c_str(), cb, xpath.c_str());
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
       
return true;
}
bool NetConfAgent::subscriberForRpc(const string &module_name)
{
    try
    {
        printf("Application will make an rpc call in %s\n", module_name.c_str());

        auto cbVals = [](sysrepo::S_Session session, const char* op_path,
        const sysrepo::S_Vals input, sr_event_t event, uint32_t request_id, sysrepo::S_Vals_Holder output) 
        {
            cout << "\n ========== RPC CALLED ==========\n" << endl;
            print_value(input->val(0));
            auto out_vals = output->allocate(1);
            out_vals->val(0)->set("/mobile-network:activate-software-image/status","output value",SR_STRING_T);
            return SR_ERR_OK;
        };
        cout << "\n ========== SUBSCRIBE TO RPC CALL ==========\n" << endl;

        _subscribe->rpc_subscribe("/mobile-network:activate-software-image", cbVals, 1);

    //    auto in_vals = std::make_shared<sysrepo::Vals>(1);

    //     in_vals->val(0)->set("/mobile-network:activate-software-image/image-name","set somthing in input ", SR_STRING_T);

    //             cout << "\n ========== START RPC CALL ==========\n" << endl;
    //     auto out_vals = m_Session->rpc_send("/mobile-network:activate-software-image", in_vals);

    //     cout << "\n ========== PRINT RETURN VALUE ==========\n" << endl;
    //     for(size_t n=0; n < out_vals->val_cnt(); ++n)
    //         print_value(out_vals->val(n));

        cout << "\n ========== END PROGRAM ==========\n" << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }

    return true;
}

bool NetConfAgent::notifySysrepo(const string &module_name)
{
    try 
    {
        libyang::S_Context ctx = _connection->get_context();
        libyang::S_Module mod = ctx->get_module(module_name.c_str());
        auto in_trees = std::make_shared<libyang::Data_Node>(ctx, "/mobile-network:test-notif", nullptr, LYD_ANYDATA_CONSTSTRING, 0);
        std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "val1", "some-value"));
        std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "val2", "some-other-value"));

        cout << "\n ========== START NOTIF TREE SEND ==========\n" << endl;

        _session->event_notif_send(in_trees);

        cout << "\n ========== END PROGRAM NOTIF==========\n" << endl;
    } 
    catch( const std::exception& e )
    {
        cout << e.what() << endl;
        return -1;
    }
}

bool NetConfAgent::changeData(const pair<string,string> &setData)
{
    try
    {
    
        cout <<"changeDAta path -----" <<  setData.first << endl ;

        _session->set_item_str(setData.first.c_str(),setData.second.c_str());
        _session->apply_changes();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
    return true;

}

}
