#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "MockNetConfAgent.hpp"

#ifndef _MOBILE_CLIENT_HPP
#define _MOBILE_CLIENT_HPP
#include "MobileClient.hpp"
#endif

using ::testing::_;
using ::testing::Return;
namespace
{
    const string BUSY = "busy";
    const string ACTIVE = "active";
    const string IDLE = "idle";
    const string name = "Max";
    const string number = "001";
    const string incNumber = "002";
}

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
    std::unique_ptr<nsMobileClient::MobileClient> _mobileClient;
    testing::StrictMock<MockNetConfAgent> *_mock;
};

TEST_F(MobileClientTest, shouldSuccedRegisterCleint)
{

    EXPECT_CALL(*_mock, initSysrepo())
        .WillOnce(Return(true));
    EXPECT_CALL(*_mock, changeData(_)).Times(2);

     //_mobileClient->setNumber(number);
    // _mobileClient->setName(name);
    _mobileClient->setState(number, IDLE);

    EXPECT_CALL(*_mock, registerOperData(_, _, _))
        .WillOnce(Return(true));
    EXPECT_CALL(*_mock, subscriberForModelChanges(_, _, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(_mobileClient->registerClient(name, number));
}

TEST_F(MobileClientTest, shouldSuccedToHandleOperData)
{
    const string tname = "testName";
    _mobileClient->setName(name);
}

TEST_F(MobileClientTest, shouldSuccedToSetName)
{

    _mobileClient->setName(name);
}

TEST_F(MobileClientTest, shouldSuccedToSetNumber)
{

    _mobileClient->setName(number);
}

TEST_F(MobileClientTest, shouldSuccedToMakeCall)
{
    EXPECT_CALL(*_mock, changeData(_)).Times(3);
    _mobileClient->setIncomigNumber(number);
    _mobileClient->setState(number, BUSY);
    _mobileClient->setState(incNumber, BUSY);
    _mobileClient->setOutNUm(incNumber);

    _mobileClient->setNumber(number);
    EXPECT_CALL(*_mock, fetchData(_, _)).WillOnce(Return(true));
    _mobileClient->makeCall(incNumber);
}

TEST_F(MobileClientTest, shouldSuccedToAnswer)
{
    EXPECT_CALL(*_mock, changeData(_)).Times(2);
    _mobileClient->setState(number, ACTIVE);
    _mobileClient->setState(incNumber, ACTIVE);
    EXPECT_CALL(*_mock, fetchData(_, _));
    _mobileClient->answer();
}

TEST_F(MobileClientTest, shouldSuccedToCallEnd)
{
    EXPECT_CALL(*_mock, changeData(_)).Times(3);
    _mobileClient->setIncomigNumber(number);
    _mobileClient->setState(number, IDLE);
    _mobileClient->setState(incNumber, IDLE);
    EXPECT_CALL(*_mock, fetchData(_, _));
    _mobileClient->callEnd();
}

TEST_F(MobileClientTest, shouldSuccedToReject)
{
     EXPECT_CALL(*_mock, changeData(_)).Times(2);
     _mobileClient->setState(number, IDLE);
     _mobileClient->setState(incNumber, IDLE);
     EXPECT_CALL(*_mock, fetchData(_, _));
     _mobileClient->reject();
}

TEST_F(MobileClientTest, shouldSuccedToUnregister)
{
     EXPECT_CALL(*_mock, fetchData(_, _));
     _mobileClient->reject();
}