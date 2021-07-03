#ifndef _NET_CONF_AGENT_HPP
#define _NET_CONF_AGENT_HPP
#include "NetConfAgent.hpp"
#endif

#include "gmock/gmock.h"

class MockNetConfAgent : public ns_NetConf::NetConfAgent
{
public:
  MOCK_METHOD(bool, initSysrepo, (), (override));
  MOCK_METHOD(bool, fetchData, (const string &xpath, (map<string, string> & mapFromFetch)), (override));
  MOCK_METHOD(bool, subscriberForModelChanges, (const string &module_name, nsMobileClient::MobileClient &mobClient, const string &xpath), (override));
  MOCK_METHOD(bool, registerOperData, (const string &module_name, const string &xpath, nsMobileClient::MobileClient &mobClient), (override));
  MOCK_METHOD(bool, subscriberForRpc, (const string &module_name), (override));
  MOCK_METHOD(bool, notifySysrepo, (const string &module_name), (override));
  MOCK_METHOD(bool, changeData, ((const pair<string, string> &setData)), (override));
  MOCK_METHOD(bool, deleteItem, (const string &path), (override));
};
