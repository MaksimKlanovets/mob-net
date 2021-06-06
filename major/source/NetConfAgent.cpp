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

bool NetConfAgent::initSysrepo( ){

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

bool NetConfAgent::fetchData(const char *xpath,libyang::S_Data_Node *data){
/* read running config */

cout << "called fetchData" << endl;
       try {
        
        libyang::S_Data_Node data = m_Session->get_data(xpath);

        /* go through all top-level siblings */
        for (libyang::S_Data_Node &root : data->tree_for()) {
            /* go through all the children of a top-level sibling */
            for (libyang::S_Data_Node &node : root->tree_dfs()) {
                print_node(node);
                
            }
        }
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

bool NetConfAgent::subscriberForModelChanges(const char *module_name, const char *xpath){
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

        /* read running config */
        cout << "\n\n ========== READING RUNNING CONFIG: ==========\n" << endl;
        print_current_config(m_Session, module_name);
}
catch( const std::exception& e ) {
        cout << e.what() << endl;
        return -1;
    }

       
}


bool NetConfAgent::registerOperData(){

   

}
bool NetConfAgent::subscriberForRpc(const char *module_name){

    // auto cbVals = [](sysrepo::S_Session session, const char* op_path, const sysrepo::S_Vals input, sr_event_t event, uint32_t request_id, sysrepo::S_Vals_Holder output) {
    //         cout << "\n ========== RPC CALLED ==========\n" << endl;

    //         auto out_vals = output->allocate(3);

    //         for(size_t n = 0; n < input->val_cnt(); ++n)
    //             print_value(input->val(n));

    //         out_vals->val(0)->set("/test-examples:activate-software-image/status",
    //                 "The image acmefw-2.3 is being installed.",
    //                 SR_STRING_T);
    //         out_vals->val(1)->set("/test-examples:activate-software-image/version",
    //                 "2.3",
    //                 SR_STRING_T);
    //         out_vals->val(2)->set("/test-examples:activate-software-image/location",
    //                 "/root/",
    //                 SR_STRING_T);

    //         return SR_ERR_OK;
    //     };

    //     auto cbTree = [] (sysrepo::S_Session session, const char *op_path, const libyang::S_Data_Node input, sr_event_t event,
    //             uint32_t request_id, libyang::S_Data_Node output) {
    //         cout << "\n ========== RPC TREE CALLED ==========\n" << endl;
    //         cout << input->print_mem(LYD_XML, LYP_FORMAT);

    //         libyang::S_Context ctx = session->get_context();

    //         output->new_path(ctx, "status", "The image acmefw-2.3 is being installed.", LYD_ANYDATA_CONSTSTRING, LYD_PATH_OPT_OUTPUT);
    //         output->new_path(ctx, "version", "2.3", LYD_ANYDATA_CONSTSTRING, LYD_PATH_OPT_OUTPUT);
    //         output->new_path(ctx, "location", "/root/", LYD_ANYDATA_CONSTSTRING, LYD_PATH_OPT_OUTPUT);

    //         return SR_ERR_OK;
    //     };

    //     cout << "\n ========== SUBSCRIBE TO RPC CALL ==========\n" << endl;

    //     subscribe->rpc_subscribe("/test-examples:activate-software-image", cbVals, 1);

    //     auto in_vals = std::make_shared<sysrepo::Vals>(2);

    //     in_vals->val(0)->set("/test-examples:activate-software-image/image-name",
    //                        "acmefw-2.3",
    //            SR_STRING_T);
    //     in_vals->val(1)->set("/test-examples:activate-software-image/location",
    //                        "/root/",
    //                        SR_STRING_T);

    //     cout << "\n ========== START RPC CALL ==========\n" << endl;
    //     auto out_vals = sess->rpc_send("/test-examples:activate-software-image", in_vals);

    //     cout << "\n ========== PRINT RETURN VALUE ==========\n" << endl;
    //     for(size_t n=0; n < out_vals->val_cnt(); ++n)
    //         print_value(out_vals->val(n));

    //     cout << "\n ========== SUBSCRIBE TO RPC TREE CALL ==========\n" << endl;
    //     subscribe->rpc_subscribe_tree("/test-examples:activate-software-image", cbTree, 0, SR_SUBSCR_CTX_REUSE);

    //     libyang::S_Context ctx = conn->get_context();
    //     libyang::S_Module mod = ctx->get_module(module_name);
    //     auto in_trees = std::make_shared<libyang::Data_Node>(ctx, "/test-examples:activate-software-image", nullptr, LYD_ANYDATA_CONSTSTRING, 0);
    //     std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "image-name", "acmefw-2.3"));
    //     std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "location", "/root/"));

    //     cout << "\n ========== START RPC TREE CALL ==========\n" << endl;
    //     auto out_trees = sess->rpc_send(in_trees);

    //     cout << "\n ========== PRINT RETURN VALUE ==========\n" << endl;
    //     cout << out_trees->print_mem(LYD_XML, LYP_FORMAT);

    //     cout << "\n ========== END PROGRAM ==========\n" << endl;
    // } catch( const std::exception& e ) {
    //     cout << e.what() << endl;
    //     return -1;
    // }
}

bool NetConfAgent::notifySysrepo(){

}

bool NetConfAgent::changeData(const char *module_name,const char *xpath, libyang::S_Data_Node *data){
//  cout << "Application will provide data of " << module_name << endl;

// auto cb1 = [] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
//             uint32_t request_id, libyang::S_Data_Node &parent) {

//             cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;

//             libyang::S_Context ctx = session->get_context();
//             libyang::S_Module mod = ctx->get_module(module_name);

//             parent.reset(new libyang::Data_Node(ctx, "/ietf-interfaces:interfaces-state", nullptr, LYD_ANYDATA_CONSTSTRING, 0));

//             libyang::S_Data_Node ifc(new libyang::Data_Node(parent, mod, "interface"));
//             libyang::S_Data_Node name(new libyang::Data_Node(ifc, mod, "name", "eth100"));
//             libyang::S_Data_Node type(new libyang::Data_Node(ifc, mod, "type", "iana-if-type:ethernetCsmacd"));
//             libyang::S_Data_Node oper_status(new libyang::Data_Node(ifc, mod, "oper-status", "down"));

//             ifc.reset(new libyang::Data_Node(parent, mod, "interface"));
//             name.reset(new libyang::Data_Node(ifc, mod, "name", "eth101"));
//             type.reset(new libyang::Data_Node(ifc, mod, "type", "iana-if-type:ethernetCsmacd"));
//             oper_status.reset(new libyang::Data_Node(ifc, mod, "oper-status", "up"));

//             ifc.reset(new libyang::Data_Node(parent, mod, "interface"));
//             name.reset(new libyang::Data_Node(ifc, mod, "name", "eth102"));
//             type.reset(new libyang::Data_Node(ifc, mod, "type", "iana-if-type:ethernetCsmacd"));
//             oper_status.reset(new libyang::Data_Node(ifc, mod, "oper-status", "dormant"));

//             ifc.reset(new libyang::Data_Node(parent, mod, "interface"));
//             name.reset(new libyang::Data_Node(ifc, mod, "name", "eth105"));
//             type.reset(new libyang::Data_Node(ifc, mod, "type", "iana-if-type:ethernetCsmacd"));
//             oper_status.reset(new libyang::Data_Node(ifc, mod, "oper-status", "not-present"));

//             return SR_ERR_OK;
//         };
//         auto cb2 = [] (sysrepo::S_Session session, const char *module_name, const char *path, const char *request_xpath,
//             uint32_t request_id, libyang::S_Data_Node &parent) {
//             cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;

//             libyang::S_Context ctx = session->get_context();
//             libyang::S_Module mod = ctx->get_module(module_name);

//             auto stats = std::make_shared<libyang::Data_Node>(parent, mod, "statistics");
//             auto dis_time = std::make_shared<libyang::Data_Node>(stats, mod, "discontinuity-time", "2019-01-01T00:00:00Z");
//             auto in_oct = std::make_shared<libyang::Data_Node>(stats, mod, "in-octets", "22");

//             return SR_ERR_OK;
//         };

//         m_Subscribe->oper_get_items_subscribe(module_name, cb1, "/ietf-interfaces:interfaces-state");
//         m_Subscribe->oper_get_items_subscribe(module_name, cb2, "/ietf-interfaces:interfaces-state/interface/statistics");
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

