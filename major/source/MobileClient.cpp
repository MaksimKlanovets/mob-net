
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

void MobileClient::handleModuleChange()
{
  cout << "called handleModuleChange" << endl;
  string pathCh = PATH +  _number + "']/incomingNumber";
  //cout << "path ____________"<<pathCh << endl;
  map<string, string>dataForFetch;
  string pathFetch = PATH +  _number + "']";
  _netConfAgent->fetchData(pathFetch, dataForFetch);


  for (map<string,string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
  {
    if (it->first != pathCh)
    {
      continue;
    }
    
      cout << "key->  " << it->first << "\n" << "value->  " << it->second <<endl;

  }
  
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
  const pair<string,string> setData = make_pair(pathCh,"idle");
  _netConfAgent->changeData(setData);

   const string pathForSubs = PATH +  _number + "']" + "/incomingNumber";
  _netConfAgent->subscriberForModelChanges(MODULE_NAME,*this,pathForSubs);

//   cout <<"//////////////////test data////////////////////"<< endl;
 
//   const string pathCh1 = PATH +  _number + "']" + "/incomingNumber";
//   const pair<string,string> setData1 = make_pair(pathCh1,"0001");

//   _netConfAgent->changeData(setData1);//called twice/ why?
// cout << "//////////end tests data////////////////////////////////////// "<< endl;

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

void MobileClient::setIncomigNumber(const string &number)
{
  const string pathCh = PATH + number + "']" + "/incomingNumber";
  const pair<string,string> setData1 = make_pair(pathCh, _number);

  _netConfAgent->changeData(setData1);
}

void  MobileClient::makeCall(const string &number)
{
    
  //if idle set 
  //if busy return client is making call without talk
  //if active client is talking to 
    setIncomigNumber(number);
}

void  MobileClient::setState(const string &state)
{
  
}


}
