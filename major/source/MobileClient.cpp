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

  string createPath(const string &key, const string &suffix = "")
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
  //how to pass struct data of client with params as stuct
  void MobileClient::handleModuleChange(const string &state, const string &incomNum, const string &incomState) //incoming num
  {
    cout << "state->" << state << " incNUm->"
         << " incStat-" << incomState << endl;
    if (incomState == BUSY && !incomNum.empty() && state == IDLE)
    {
      cout << "incoming number " << incomNum << endl;
    }
    else if (incomState == ACTIVE && state == BUSY)
    {
      cout << "number " << incomNum << " picked up phone" << endl;
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
        !_netConfAgent->subscriberForModelChanges(MODULE_NAME, *this, createPath(_number, PATH_INC_NUM)))
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
    const pair<string, string> setData1 = make_pair(createPath(number, PATH_INC_NUM), _number);

    _netConfAgent->changeData(setData1);
  }

  void MobileClient::makeCall(const string &number)
  {
    setState(_number, BUSY);

    map<string, string> dataForFetch;
    _netConfAgent->fetchData(createPath(number), dataForFetch);
    const pair<string, string> setData = make_pair(createPath(_number, PATH_STATE), IDLE);

    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {
      //if busy return client is making call without talk
      if (it->second == BUSY)
      {
        cout << "client is busy now" << endl;
        const pair<string, string> setData = make_pair(createPath(_number, PATH_STATE), IDLE);
        _netConfAgent->changeData(setData);
      }
      //if active client is talking to
      else if (it->second == ACTIVE)
      {
        cout << "client is talking" << endl;
        _netConfAgent->changeData(setData);
      }
      else if (it->second == IDLE)
      {
        if (dataForFetch.find(createPath(number, PATH_INC_NUM)) == dataForFetch.end())
        {
          setIncomigNumber(number);
          const pair<string, string> setData1 = make_pair(createPath(_number, PATH_INC_NUM), number);

          _netConfAgent->changeData(setData1);
        }
        else
        {
          cout << "line is busy" << endl;
          _netConfAgent->changeData(setData);
        }
      }
    }
  }
  void MobileClient::answer()
  {

    map<string, string> dataForFetch;
    _netConfAgent->fetchData(createPath(_number), dataForFetch);

    string incomingNumber;
    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {
      if (it->first != createPath(_number, PATH_INC_NUM))
      {
        continue;
      }
      incomingNumber = it->second;
      cout << incomingNumber << endl;

      _netConfAgent->deleteItem(createPath(incomingNumber, PATH_INC_NUM));
      cout << "path " << createPath(incomingNumber, PATH_INC_NUM) << endl;

      setState(_number, ACTIVE);
      setIncomigNumber(incomingNumber);

      setState(incomingNumber, ACTIVE);

      break;
    }
  }

  void MobileClient::callEnd()
  {
    //change status in both /wipe out incoming numbers
    map<string, string> dataForFetch;
    // string pathFetch = PATH + _number + "']";
    _netConfAgent->fetchData(createPath(_number), dataForFetch);
    string incomingNumber = {};

    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {
      if (it->first != createPath(_number, PATH_INC_NUM))
      {
        continue;
      }
      incomingNumber = it->second;
      _netConfAgent->deleteItem(createPath(_number, PATH_INC_NUM));
      _netConfAgent->deleteItem(createPath(incomingNumber, PATH_INC_NUM));
      //set status
      setState(_number, IDLE);
      // set status incoming
      setState(incomingNumber, IDLE);
      cout << "erased _incom" << incomingNumber << endl;
      break;
    }
  }

  void MobileClient::setState(const string &number, const string &status)
  {
    //cout << "called set state" << endl;
    const pair<string, string> setData = make_pair(createPath(number, PATH_STATE), status);
    _netConfAgent->changeData(setData);
  }

  void MobileClient::reject()
  {
    //cout << "called reject" << endl;
    map<string, string> dataForFetch;
    _netConfAgent->fetchData(createPath(_number), dataForFetch);
    string incomingNumber;
    for (map<string, string>::const_iterator it = dataForFetch.begin(); it != dataForFetch.end(); it++)
    {

      if (it->first != createPath(_number, PATH_INC_NUM))
      {
        continue;
      }
      cout << "incom num   " << incomingNumber;
      incomingNumber = it->second;
      setState(incomingNumber, IDLE);
      setState(_number, IDLE);

      _netConfAgent->deleteItem(createPath(_number, PATH_INC_NUM));
      _netConfAgent->deleteItem(createPath(incomingNumber, PATH_INC_NUM));
    }
  }

  void MobileClient::unregister()
  {
    _netConfAgent->deleteItem(createPath(_number));

    _netConfAgent->~NetConfAgent();
  }
  string MobileClient::getNumber()
  {
    return _number;
  }

}