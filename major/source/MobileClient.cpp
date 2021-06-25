
#ifndef _MOBILE_CLIENT_HPP
#define _MOBILE_CLIENT_HPP
#include "MobileClient.hpp"
#endif 
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
  void MobileClient::handleModuleChange(const string &state, const string &incomNum, const string &incomState) //incoming num
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
        !_netConfAgent->subscriberForModelChanges(MODULE_NAME, *this, createPath(_number, PATH_STATE)))
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
    map<string, string> dataFor;

    if (_netConfAgent->fetchData(createPath(number), dataFor) && number != _number)
    {
      setIncomigNumber(number);
      map<string, string> dataForFetch;
      const pair<string, string> setData = make_pair(createPath(_number, PATH_STATE), IDLE);
      _netConfAgent->fetchData(createPath(number), dataForFetch);

      for (auto it : dataForFetch)
      {
        if (it.second == BUSY)
        {
          cout << "client is busy now" << endl;
          const pair<string, string> setData = make_pair(createPath(_number, PATH_STATE), IDLE);
          _netConfAgent->changeData(setData);
        }
        else if (it.second == ACTIVE)
        {
          cout << "client is talking" << endl;
          _netConfAgent->changeData(setData);
        }
        else if (it.second == IDLE)
        {
          setOutNUm(number);
          setState(_number, BUSY);
          setState(number, BUSY);
        }
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
    _netConfAgent->fetchData(createPath(_number, PATH_INC_NUM), dataForFetch);
    auto it = dataForFetch.find(createPath(_number, PATH_INC_NUM));
    setState(_number, ACTIVE);
    setState(it->second, ACTIVE);
  }

  void MobileClient::callEnd()
  {
    map<string, string> dataForFetch;
    string temp{_outNum};
    _outNum.clear();
    _netConfAgent->fetchData(createPath(_number, PATH_INC_NUM), dataForFetch);
    auto it = dataForFetch.find(createPath(_number, PATH_INC_NUM));
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
    _netConfAgent->fetchData(createPath(_number, PATH_INC_NUM), dataForFetch);
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
}