
#include "MobileClient.hpp"

  using namespace std;

 namespace
 {
   const string MODULE_NAME = "mobile-network";
   const string PATH = "/mobile-network:core/subscribers[number='";
 }

 namespace nsMobileClient
 {

 MobileClient::MobileClient(/* args */)
 {
   _name = {};
   _number = {};
 }
 
 MobileClient::~MobileClient()
 {
 }
// const module_name/ path
 bool MobileClient::registerClient(const string &name, const string &number)
 {

  cout << "called registerClient" <<endl;

  _netConfAgent = make_shared<ns_NetConf::NetConfAgent>();
  _netConfAgent->initSysrepo();

  //set user name and number
  _name = name;
  _number = number;
    
  const string path = PATH +  _number + "']";

  _netConfAgent->registerOperData(MODULE_NAME,path,*this);

  cout <<"register end" <<endl;
  return true;

 }

void MobileClient::handleOperData(string &name)
{
 cout << " called handleOperData" << endl;
 name = _name;
}

}