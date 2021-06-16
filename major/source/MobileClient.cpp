
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

 bool MobileClient::registerClient(const string &name, const string &number)
 {

  cout << "called registerClient" <<endl;

  _netConfAgent = make_shared<ns_NetConf::NetConfAgent>();
  _netConfAgent->initSysrepo();

  setNumber(number);
  setName(name);
  
  //set state - idle
  const string pathCh = PATH +  _number + "']" + "/state";
  pair<string,string> setData = make_pair(pathCh,"idle");
  _netConfAgent->changeData(setData);
  
  cout <<"register end" <<endl;
  return true;

 }

void MobileClient::handleOperData(string &name)
{
 cout << " called handleOperData" << endl;
 name = _name;
}

void MobileClient::setName(const string &name)
{
  _name = name;
  const string path = PATH +  _number + "']";
  _netConfAgent->registerOperData(MODULE_NAME,path,*this); 
}
void MobileClient::setNumber(const string &number)
{
_number = number;
}

}