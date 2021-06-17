
#include "MobileClient.hpp"

  using namespace std;

 namespace
 {
   const string MODULE_NAME = "mobile-network";
   const string PATH = "/mobile-network:core/subscribers[number='";
   const string BUSY = "busy";
   const string ACTIVE = "active";
   const string IDLE = "idle";
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
  //cout << "called handleModuleChange" << endl;
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
    //  cout << "key->  " << it->first << "\n" << "value->  " << it->second <<endl;
      cout << "incoming number " << it->second <<endl;
  }
  
}

 bool MobileClient::registerClient(const string &name, const string &number)
 {

  cout << "called registerClient" <<endl;

  _netConfAgent = make_shared<ns_NetConf::NetConfAgent>();
  _netConfAgent->initSysrepo();

  setNumber(number);
  setName(name);
  
  const string path = PATH +  _number + "']";
  _netConfAgent->registerOperData(MODULE_NAME,path,*this); 
  
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
 //cout << " called handleOperData" << endl;
 name = _name;
}

void MobileClient::setName(const string &name)
{
  _name = name;
  
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
  //set status busy
  const string pathCh = PATH +  _number + "']" + "/state";
 // cout << "called make call " << pathCh <<  endl;
  const pair<string,string> setData = make_pair(pathCh,BUSY);
  _netConfAgent->changeData(setData);

  
  map<string, string>dataForFetch;
  string pathFetch = PATH +  number + "']";
  _netConfAgent->fetchData(pathFetch, dataForFetch);

  for (map<string,string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
   {
    if (it->first != pathFetch + "/state")
    {
      continue;
    }
    //if busy return client is making call without talk
     if (it->second == BUSY)
    {
      cout << "client is busy now" << endl;
      const pair<string,string> setData = make_pair(pathCh,IDLE);
      _netConfAgent->changeData(setData);
      
    } 
    //if active client is talking to
    else if (it->second == ACTIVE)
    {
      cout << "client is talking" << endl;
       const pair<string,string> setData = make_pair(pathCh,IDLE);
      _netConfAgent->changeData(setData);
    }
    else if (it->second == IDLE)
    {
      //if idle set 
      setIncomigNumber(number);
      
    }
    //cout << "key->  " << it->first << "\n" << "value->  " << it->second <<endl;
  }
    
}
void  MobileClient::answer()
{
  map<string, string>dataForFetch;
  string pathFetch = PATH +  _number + "']";
  _netConfAgent->fetchData(pathFetch, dataForFetch);
  string incomingNumber;

  for (map<string,string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
   {
    if (it->first != pathFetch + "/incomingNumber" )
    {
      continue;
    }
    incomingNumber = it->second;
    
    cout << incomingNumber <<endl;
   }
   //set status 
  const string pathCh = PATH +  _number + "']" + "/state";
  const pair<string,string> setData = make_pair(pathCh,ACTIVE);
  _netConfAgent->changeData(setData);
  // set status incoming 
  const string pathChincoming = PATH +  incomingNumber + "']" + "/state";
  const pair<string,string> setDataOutgoing = make_pair(pathChincoming,ACTIVE);
  _netConfAgent->changeData(setDataOutgoing );

  //how to exclede it?  
  //it's needs in order to find second side in func callEnd[incoming number] change state it both of them 
  //set the incoming number to the caller
  const string pathChIncomCaller = PATH +  incomingNumber + "']" + "/incomingNumber";
  //cout << "path for incom"<< pathChIncomCaller << endl;
  const pair<string,string> IncomCaller = make_pair(pathChIncomCaller,_number);
  _netConfAgent->changeData(IncomCaller);
}

void MobileClient::callEnd()
{
//change status in both /wipe out incoming numbers 
 map<string, string>dataForFetch;
  string pathFetch = PATH +  _number + "']";
  _netConfAgent->fetchData(pathFetch, dataForFetch);
  string incomingNumber;

  for (map<string,string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
   {
    if (it->first != pathFetch + "/incomingNumber" )
    {
      continue;
    }
    incomingNumber = it->second;
    
    cout << incomingNumber <<endl;
   }
   
  //set status 
  const string pathCh = PATH +  _number + "']" + "/state";
  const pair<string,string> setData = make_pair(pathCh,IDLE);
  _netConfAgent->changeData(setData);
  // set status incoming 
  const string pathChincoming = PATH +  incomingNumber + "']" + "/state";
  const pair<string,string> setDataOutgoing = make_pair(pathChincoming,IDLE);
  _netConfAgent->changeData(setDataOutgoing );
  
 //how to wipe inconmingNumbers out?
}

void  MobileClient::setState(const pair<string,string> &setData)
{
  cout << "called set state" << endl;
}


}
