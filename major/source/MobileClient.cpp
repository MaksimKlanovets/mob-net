
#include "MobileClient.hpp"

using namespace std;

namespace
{
  const string MODULE_NAME = "mobile-network";
  const string PATH = "/mobile-network:core/subscribers[number='";
  const string BUSY = "busy";
  const string ACTIVE = "active";
  const string IDLE = "idle";
  const string PATH_INC_NUM = "/incomingNumber";
  const string PATH_STATE = "/state";
  
  string createPath(const string& key,const string& suffix = "" )
  {
    return PATH + key + "']" + suffix;
  }
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

  void MobileClient::handleModuleChange(const string& state,const string& incomNum, const string& incomState  ) //incoming num
  {
    //cout << "called handleModuleChange" << endl;
    if (incomState == BUSY && !incomNum.empty() && state == IDLE)
    {
      cout << "incoming number " << incomNum << endl;
    }
    else if (incomState == ACTIVE && state == ACTIVE)
    {
      cout << "call end" << endl;
    }
    else if (state == IDLE && !incomNum.empty() && incomState == IDLE)
    {
      cout << "reject" << endl;
    }
  }

  bool MobileClient::registerClient(const string &name)
  {
    _netConfAgent = make_shared<ns_NetConf::NetConfAgent>();
    
    if (!_netConfAgent->initSysrepo())
    {
      cout << "something with connection" << endl;
    }
    
    setName(name);
    setState(_number, IDLE);

    if (!_netConfAgent->registerOperData(MODULE_NAME, createPath(_number), *this) ||
    !_netConfAgent->subscriberForModelChanges(MODULE_NAME, *this, createPath(_number,PATH_INC_NUM)))
    {
      return false;
    }
    
    cout << "registration completed" << endl;

    return true;
  }

  void MobileClient::handleOperData(string &name)
  {
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
    const pair<string, string> setData1 = make_pair(pathCh, _number);

    _netConfAgent->changeData(setData1);
  }

  void MobileClient::makeCall(const string &number)
  {
    //set status busy
    setState(_number, BUSY);
    const string pathCh = PATH + _number + "']" + "/state";

    map<string, string> dataForFetch;
    string pathFetch = PATH + number + "']";
    _netConfAgent->fetchData(pathFetch, dataForFetch);

    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {
      if (it->first != pathFetch + "/state")
      {
        continue;
      }

      //if busy return client is making call without talk
      if (it->second == BUSY)
      {
        cout << "client is busy now" << endl;
        const pair<string, string> setData = make_pair(pathCh, IDLE);
        _netConfAgent->changeData(setData);
      }
      //if active client is talking to
      else if (it->second == ACTIVE)
      {
        cout << "client is talking" << endl;
        const pair<string, string> setData = make_pair(pathCh, IDLE);
        _netConfAgent->changeData(setData);
      }
      else if (it->second == IDLE)
      {
        //if idle set
        // setPrivateIncomNum(number);
        setIncomigNumber(number);

        const string path = PATH + _number + "']" + "/incomingNumber";
        const pair<string, string> setData1 = make_pair(path, number);
        _netConfAgent->changeData(setData1);
        // cout << "set Incoming number" << endl;
      }
    }
  }
  void MobileClient::answer()
  {

    map<string, string> dataForFetch;
    string pathFetch = PATH + _number + "']";
    _netConfAgent->fetchData(pathFetch, dataForFetch);

    string incomingNumber;
    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {
      if (it->first != pathFetch + "/incomingNumber")
      {
        continue;
      }
      incomingNumber = it->second;

     // cout << incomingNumber << endl;
      setIncomigNumber(incomingNumber);
    }
    //set status
    setState(_number, ACTIVE);
    //cout << "_number" << _number<< endl;
    setState(incomingNumber, ACTIVE);
    //cout << "_incoming number in F_answer" << incomingNumber<< endl;
  }

  void MobileClient::callEnd()
  {
    //change status in both /wipe out incoming numbers
    map<string, string> dataForFetch;
    string pathFetch = PATH + _number + "']";
    _netConfAgent->fetchData(pathFetch, dataForFetch);
    string incomingNumber = {};

    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {
      if (it->first != pathFetch + "/incomingNumber")
      {
        continue;
      }
      incomingNumber = it->second;

      cout << incomingNumber << endl;
    }

    string pathDelItem1 = PATH + _number + "']/incomingNumber";
    string pathDelItem = PATH + incomingNumber + "']/incomingNumber";

    //  cout << pathDelItem1 << "<-path for delete" << endl;
    //  cout << pathDelItem << "<-path for delete" << endl;

    _netConfAgent->deleteItem(pathDelItem1);
    _netConfAgent->deleteItem(pathDelItem);
    //set status
    setState(_number, IDLE);
    // set status incoming
    setState(incomingNumber, IDLE);
    //cout << "erased _incom" << incomingNumber << endl;
  }

  void MobileClient::setState(const string &number, const string &status)
  {
    //cout << "called set state" << endl;
    const string pathCh = PATH + number + "']" + "/state";
    const pair<string, string> setData = make_pair(pathCh, status);
    _netConfAgent->changeData(setData);
  }

  void MobileClient::reject()
  {
    //cout << "called reject" << endl;
    string pathCh = PATH + _number + "']";
    map<string, string> dataForFetch;
    _netConfAgent->fetchData(pathCh, dataForFetch);
    string incomingNumber;
    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {

      if (it->first == pathCh + "/incomingNumber")
      {
        incomingNumber = it->second;
      }
    }

    string pathDelItem1 = PATH + _number + "']/incomingNumber";
    string pathDelItem = PATH + incomingNumber + "']/incomingNumber";

    setState(incomingNumber, IDLE);
    setState(_number, IDLE);

    _netConfAgent->deleteItem(pathDelItem1);
    _netConfAgent->deleteItem(pathDelItem);

   // cout << "incom num" << incomingNumber;
  }

  void MobileClient::unregister()
  {
    string pathDelItem = PATH + _number + "']";
    _netConfAgent->deleteItem(pathDelItem);

    _netConfAgent->~NetConfAgent();
  }
  string MobileClient::getNumber()
  {
    return _number;
  }

}