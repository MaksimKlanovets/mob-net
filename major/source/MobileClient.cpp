 #include "MobileClient.hpp"
 
 using namespace std;
 namespace nsMobileClient
 {

 MobileClient::MobileClient(/* args */)
 {
   _name = {};
 }
 
 MobileClient::~MobileClient()
 {
 }

 bool MobileClient::registerClient(const string &module_name, const pair<string,string> &setData)
 {

    cout << "called registerClient" <<endl;
      try
     {
        _netConfAgent = make_shared<ns_NetConf::NetConfAgent>();
        _netConfAgent->initSysrepo();

     }
     catch(const std::exception& e)
     {
         std::cerr << e.what() << '\n';
         return false;
     } 
    _name = setData.second;
   
    _netConfAgent->registerOperData(module_name,setData);
      
    cout <<"register end" <<endl;
     return true;

 }

 }

 