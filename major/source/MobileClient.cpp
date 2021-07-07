#include "MobileClient.hpp"

using std::cout;
namespace
{
  const string MODULE_NAME = "mobile-network";
  const string PATH = "/mobile-network:core/subscribers[number='";
  const string BUSY = "busy";
  const string ACTIVE = "active";
  const string IDLE = "idle";
  const string PATH_INC_NUM = "/incomingNumber";
  const string PATH_STATE = "/state";
}

namespace nsMobileClient
{
  MobileClient::MobileClient()
      : MobileClient::MobileClient(make_unique<ns_NetConf::NetConfAgent>())
  {
  }

  MobileClient::MobileClient(unique_ptr<ns_NetConf::NetConfAgent> netConfAgent)
      : _isReg{false}, _netConfAgent{move(netConfAgent)}
  {
  }

  void MobileClient::handleModuleChange(const string &state, const string &incomNum, const string &incomState)
  {
    if (state == BUSY && incomState == BUSY)
    {
      cout << "incoming number " << incomNum << endl;
    }
    else if (state == IDLE && !incomNum.empty() && incomState == IDLE ||
             state == IDLE && !_outNum.empty() && incomNum.empty())
    {
      cout << "reject called " << endl;
    }
    else if (state == IDLE && incomNum.empty() ||
             state == IDLE && !_outNum.empty() && !incomNum.empty())
    {
      cout << "callEnd called " << endl;
    }
    else if (state == ACTIVE)
    {
      cout << "answer called" << endl;
    }
  }

  bool MobileClient::registerClient(const string &name, const string &number)
  {

    _netConfAgent->initSysrepo();
    setNumber(number);
    setName(name);
    setState(_number, IDLE);

    if (!_netConfAgent->registerOperData(MODULE_NAME, createPath(_number), *this) ||
        !_netConfAgent->subscriberForModelChanges(MODULE_NAME, *this, createPath(_number, PATH_STATE)))
    {
      return false;
    }

    _isReg = true;
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
    map<string, string> dataFor;
    map<string, string> dataForUser;
    _netConfAgent->fetchData(createPath(_number), dataForUser);
    auto itUserState = dataForUser.find(createPath(_number, PATH_STATE));

    if (itUserState->second == IDLE)
    {
      _outNum.clear();
    }
    if (itUserState->second != IDLE)
    {
      cout << "not allowed" << endl;
      return;
    }
    if (_netConfAgent->fetchData(createPath(number), dataFor) && number != _number)
    {
      auto itDataFor = dataFor.find(createPath(number, PATH_STATE));

      if (itDataFor->second == BUSY)
      {
        cout << "client is busy now" << endl;
      }
      else if (itDataFor->second == ACTIVE)
      {
        cout << "client is talking" << endl;
      }
      else if (itDataFor->second == IDLE)
      {
        setIncomigNumber(number);
        setOutNUm(number);
        setState(_number, BUSY);
        setState(number, BUSY);
      }
    }
    else
    {
      cout << "number does not exist" << endl;
    }
  }
  void MobileClient::answer()
  {
    map<string, string> dataForFetch;

    if (_netConfAgent->fetchData(createPath(_number, PATH_INC_NUM), dataForFetch))
    {
      _outNum.clear();
      auto it = dataForFetch.find(createPath(_number, PATH_INC_NUM));
      setState(_number, ACTIVE);
      setState(it->second, ACTIVE);
    }
    else
    {
      cout << "no incoming number, command is not correct" << endl;
    }
  }

  void MobileClient::callEnd()
  {
    map<string, string> dataForFetch;
    _netConfAgent->fetchData(createPath(_number), dataForFetch);
    auto it = dataForFetch.find(createPath(_number, PATH_INC_NUM));
    auto it1 = dataForFetch.find(createPath(_number, PATH_STATE));
    string temp{_outNum};
    _outNum.clear();

    if (it1->second != ACTIVE)
    {
      cout << "no active phone call, operation not available " << endl;
      return;
    }
    if (it != dataForFetch.end())
    {
      setIncomigNumber(it->second);
      _netConfAgent->deleteItem(createPath(_number, PATH_INC_NUM));
      setState(it->second, IDLE);
      _netConfAgent->deleteItem(createPath(it->second, PATH_INC_NUM));
      setState(_number, IDLE);
    }
    else
    {
      _netConfAgent->deleteItem(createPath(temp, PATH_INC_NUM));
      setState(temp, IDLE);
      setState(_number, IDLE);
    }
  }

  void MobileClient::setState(const string &number, const string &status)
  {
    const pair<string, string> setData = make_pair(createPath(number, PATH_STATE), status);
    _netConfAgent->changeData(setData);
  }

  void MobileClient::reject()
  {
    map<string, string> dataForFetch;
    _netConfAgent->fetchData(createPath(_number), dataForFetch);
    auto itState = dataForFetch.find(createPath(_number, PATH_STATE));

    if (itState->second != BUSY)
    {
      cout << "command is not correct" << endl;
      return;
    }

    auto it = dataForFetch.find(createPath(_number, PATH_INC_NUM));

    if (it != dataForFetch.end())
    {
      setState(it->second, IDLE);
      setState(_number, IDLE);
      _netConfAgent->deleteItem(createPath(_number, PATH_INC_NUM));
    }
    else
    {
      setState(_number, IDLE);
      setState(_outNum, IDLE);
      _netConfAgent->deleteItem(createPath(_outNum, PATH_INC_NUM));
    }

    _outNum.clear();
  }

  bool MobileClient::unregister()
  {
    map<string, string> dataForFetch;
    _netConfAgent->fetchData(createPath(_number), dataForFetch);
    auto itState = dataForFetch.find(createPath(_number, PATH_STATE));

    if (itState->second != IDLE)
    {
      return false;
    }

    _netConfAgent->deleteItem(createPath(_number));
    _netConfAgent->~NetConfAgent();
    _isReg = false;
    _outNum.clear();
    return true;
  }

  void MobileClient::setOutNumber(const string &outName)
  {
    _outNum = outName;
  }

  string MobileClient::getOutNumber() const
  {
    return _outNum;
  }

  string MobileClient::getNumber()
  {
    return _number;
  }

  void MobileClient::setOutNUm(const string &outNum)
  {
    _outNum = outNum;
  }

  string MobileClient::getName() const
  {
    return _name;
  }

  string MobileClient::createPath(const string &key, const string &suffix)
  {
    return PATH + key + "']" + suffix;
  }
  void MobileClient::setIsReg(const bool &setStatus)
  {
    _isReg = setStatus;
  }

  bool MobileClient::getIsReg() const
  {
    return _isReg;
  }

}