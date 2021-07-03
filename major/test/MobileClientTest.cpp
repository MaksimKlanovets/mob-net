#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "MockNetConfAgent.hpp"
#include "MobileClient.hpp"

using ::testing::_;
using ::testing::Return;
namespace
{
    const string BUSY = "busy";
    const string ACTIVE = "active";
    const string IDLE = "idle";
    const string NAME = "Max";
    const string NUMBER = "001";
    const string INC_NUMBER = "002";
    const string PATH = "/mobile-network:core/subscribers[number='";

    const string PATH_INC_NUM = "/incomingNumber";
    const string PATH_STATE = "/state";
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

    string createPath(const string &key, const string &suffix = "")
    {
        return PATH + key + "']" + suffix;
    }

protected:
    std::unique_ptr<nsMobileClient::MobileClient> _mobileClient;
    testing::StrictMock<MockNetConfAgent> *_mock;
};

TEST_F(MobileClientTest, shouldSuccedRegisterCleint)
{

    EXPECT_CALL(*_mock, initSysrepo())
        .WillOnce(Return(true));
    EXPECT_CALL(*_mock, changeData(_)).Times(1);
    EXPECT_CALL(*_mock, registerOperData(_, _, _))
        .WillOnce(Return(true));
    EXPECT_CALL(*_mock, subscriberForModelChanges(_, _, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(_mobileClient->registerClient(NAME, NUMBER));
}

TEST_F(MobileClientTest, shouldSuccedToHandleOperData)
{
    _mobileClient->setName(NAME);
    string tname = "testName";
    _mobileClient->handleOperData(tname);
    if (NAME != tname)
    {
        cout << "incName " << NAME << " outgoing " << tname << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccedToSetName)
{
    _mobileClient->setName(NAME);
    string tName = _mobileClient->getName();
    if (NAME != tName)
    {
        cout << "incName->" << NAME << ";\toutgoing->" << tName << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccedToSetNumber)
{
    _mobileClient->setNumber(NUMBER);
    string tNumber = _mobileClient->getNumber();
    if (NUMBER != tNumber)
    {
        cout << "incNumber->" << NUMBER << ";\toutgoing->" << tNumber << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccedToSetState)
{
    EXPECT_CALL(*_mock, changeData(_)).Times(1);
    _mobileClient->setState(NUMBER, ACTIVE);
}

TEST_F(MobileClientTest, shouldSuccedToMakeCall)
{
  
    EXPECT_CALL(*_mock, fetchData(_, _));
    _mobileClient->makeCall(INC_NUMBER);
}

TEST_F(MobileClientTest, shouldSuccedToAnswer)
{
    EXPECT_CALL(*_mock, changeData(_)).Times(2);
    _mobileClient->setState(NUMBER, ACTIVE);
    _mobileClient->setState(INC_NUMBER, ACTIVE);
    EXPECT_CALL(*_mock, fetchData(_, _));
    _mobileClient->answer();
}

TEST_F(MobileClientTest, shouldSuccedToCallEnd)
{
    EXPECT_CALL(*_mock, changeData(_)).Times(3);
    _mobileClient->setIncomigNumber(NUMBER);
    _mobileClient->setState(NUMBER, IDLE);
    _mobileClient->setState(INC_NUMBER, IDLE);
    EXPECT_CALL(*_mock, fetchData(_, _));
    _mobileClient->callEnd();
}

TEST_F(MobileClientTest, shouldSuccedToReject)
{
     EXPECT_CALL(*_mock, changeData(_)).Times(2);
     _mobileClient->setState(NUMBER, IDLE);
     _mobileClient->setState(INC_NUMBER, IDLE);
     EXPECT_CALL(*_mock, fetchData(_, _));
     _mobileClient->reject();
}

TEST_F(MobileClientTest, shouldSuccedToUnregister)
{
    _mobileClient->setNumber(NUMBER);
    EXPECT_CALL(*_mock, fetchData(_, _)).Times(2);
    _mobileClient->reject();
    _mobileClient->unregister();
}

TEST_F(MobileClientTest, shouldSuccedToGetName)
{
    _mobileClient->setName(NAME);
    string tname = _mobileClient->getName();
    if (tname != NAME)
    {
        cout << "error, name does not match " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccedToCreatePath)
{
    string path = _mobileClient->createPath(NUMBER);
    if (path != (PATH + NUMBER +"']"))
    {
        cout  << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccedToSetIsReg)
{
_mobileClient->setIsReg(true);
bool state = _mobileClient->getIsReg();
if (!state)
{
    cout  << " --inconsistency " << endl;
}
}

TEST_F(MobileClientTest, shouldSuccedToGetIsReg)
{
_mobileClient->setIsReg(true);
bool state = _mobileClient->getIsReg();
if (!state)
{
    cout  << " --inconsistency " << endl;
}
}
