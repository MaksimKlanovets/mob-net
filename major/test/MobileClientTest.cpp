#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "MockNetConfAgent.hpp"

#ifndef _MOBILE_CLIENT_HPP
#define _MOBILE_CLIENT_HPP
#include "MobileClient.hpp"
#endif
#ifndef _NET_CONF_AGENT_HPP
#define _NET_CONF_AGENT_HPP
#include "NetConfAgent.hpp"
#endif

class MobileClientTest : public testing::Test
{
protected:
void SetUp() override
{
    auto tMock = std::make_unique<testing::StrictMock<MockNetConfAgent>>();
     _mock = tMock.get();
 _mobileClient = std::make_unique<nsMobileClient::MobileClient>(std::move(tMock));

}


protected:
 std::unique_ptr<nsMobileClient::MobileClient> _mobileClient ;
 testing::StrictMock<MockNetConfAgent> *_mock;

};


TEST_F(MobileClientTest, test1)
{
    std::string name = "testName";
_mobileClient->setName(name);
EXPECT_EQ(_mobileClient->getName(),"testName");
}

TEST_F(MobileClientTest, test2)
{
    std::string name = "test";
_mobileClient->setName(name);
EXPECT_EQ(_mobileClient->getName(),"tes");
}

