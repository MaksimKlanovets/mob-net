#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "MockNetConfAgent.hpp"
#include "MobileClient.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;

namespace
{
    const string NAME = "Max";
    const string NUMBER = "001";
    const string NAME_TWO = "MAT";
    const string NUMBER_TWO = "002";
    const string BUSY = "busy";
    const string IDLE = "idle";
    const string ACTIVE = "active";
    const string PATH_STATE = "/state";
    const string MODULE_NAME = "mobile-network";
    const string PATH_INC_NUM = "/incomingNumber";
    const string PATH = "/mobile-network:core/subscribers[number='";
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

    void mockRegisterClient(const string &name, const string &number)
    {
        EXPECT_CALL(*_mock, changeData(_)).Times(1);
        _mobileClient->setNumber(number);
        _mobileClient->setName(name);
        EXPECT_CALL(*_mock, initSysrepo())
            .WillOnce(Return(true));
        EXPECT_CALL(*_mock, registerOperData(MODULE_NAME, createPath(number), _))
            .WillOnce(Return(true));
        EXPECT_CALL(*_mock, subscriberForModelChanges(MODULE_NAME, _, createPath(number, PATH_STATE)))
            .WillOnce(Return(true));
        _mobileClient->registerClient(name, number);
    }

protected:
    std::unique_ptr<nsMobileClient::MobileClient> _mobileClient;
    testing::StrictMock<MockNetConfAgent> *_mock;
};

TEST_F(MobileClientTest, shouldSuccessedhandleModuleChange)
{
    const string noState = "";
    _mobileClient->handleModuleChange(BUSY,noState, BUSY);
}

TEST_F(MobileClientTest, shouldSuccessedhandleModuleChange_1)
{
    _mobileClient->handleModuleChange(IDLE,NUMBER, IDLE);
}

TEST_F(MobileClientTest, shouldSuccessedhandleModuleChange_2)
{
     const string noState = "";
    _mobileClient->handleModuleChange(IDLE,noState, noState);
}

TEST_F(MobileClientTest, shouldSuccessedhandleModuleChange_3)
{
     const string noState = "";
    _mobileClient->handleModuleChange(ACTIVE,noState, noState);
}

TEST_F(MobileClientTest, shouldSuccessedRegisterCleint)
{
    mockRegisterClient(NAME, NUMBER);
}

TEST_F(MobileClientTest, shouldSuccessedToHandleOperData)
{
    mockRegisterClient(NAME, NUMBER);
    string tname = "testName";
    _mobileClient->handleOperData(tname);
    if (NAME != tname)
    {
        cout << "incName " << NAME << " outgoing " << tname << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccessedToSetName)
{
    mockRegisterClient(NAME, NUMBER);
    _mobileClient->setName(NAME_TWO);
    string tName = _mobileClient->getName();
    if (NAME == tName)
    {
        cout << "incName->" << NAME << ";\toutgoing->" << tName << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccessedToSetNumber)
{
    mockRegisterClient(NAME, NUMBER);
    _mobileClient->setNumber(NUMBER_TWO);
    string tNumber = _mobileClient->getNumber();
    if (NUMBER == tNumber)
    {
        cout << "incNumber->" << NUMBER << ";\toutgoing->" << tNumber << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccessedToSetIncomingNumber)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER_TWO, PATH_INC_NUM), NUMBER);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    _mobileClient->setIncomigNumber(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldSuccessedToMakeCall)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER_TWO, PATH_INC_NUM), NUMBER);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    const pair<string, string> setData1 = make_pair(createPath(NUMBER, PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, changeData(setData1)).Times(1);
    const pair<string, string> setData2 = make_pair(createPath(NUMBER_TWO, PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, changeData(setData2)).Times(1);
    map<string, string> dataForOne;
    map<string, string> dataForTwo;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
    dataForTwo.emplace(createPath(NUMBER_TWO, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, fetchData(createPath(NUMBER_TWO), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForTwo), Return(true)));
    _mobileClient->makeCall(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldFailedToMakeCall)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    _mobileClient->makeCall(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldFailedToMakeCall_1)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    map<string, string> dataForTwo;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
    dataForTwo.emplace(createPath(NUMBER_TWO, PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, fetchData(createPath(NUMBER_TWO), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForTwo), Return(true)));
    _mobileClient->makeCall(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldFailedToMakeCall_2)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    map<string, string> dataForTwo;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
    dataForTwo.emplace(createPath(NUMBER_TWO, PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, fetchData(createPath(NUMBER_TWO), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForTwo), Return(true)));
    _mobileClient->makeCall(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldFailedToMakeCall_3)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    _mobileClient->makeCall(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldFailedToMakeCall_4)
{
    const string notExistingNumber = "1234";
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    map<string, string> dataForTwo;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, fetchData(createPath(NUMBER_TWO), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForTwo), Return(false)));
    _mobileClient->makeCall(NUMBER_TWO);
}

TEST_F(MobileClientTest, shouldSuccessedToAnswer)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER, PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    const pair<string, string> setData1 = make_pair(createPath(NUMBER_TWO, PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, changeData(setData1)).Times(1);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_INC_NUM), NUMBER_TWO);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber(),PATH_INC_NUM), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    _mobileClient->answer();
}

TEST_F(MobileClientTest, shouldFailedToAnswer)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber(),PATH_INC_NUM), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(false)));
    _mobileClient->answer();
}

TEST_F(MobileClientTest, shouldSuccessedToCallEnd)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER_TWO, PATH_INC_NUM), NUMBER);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    const pair<string, string> setData1 = make_pair(createPath(NUMBER_TWO, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData1)).Times(1);
    const pair<string, string> setData2 = make_pair(createPath(NUMBER, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData2)).Times(1);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_INC_NUM), NUMBER_TWO);
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getNumber(), PATH_INC_NUM)))
        .WillOnce(Return(true));
    EXPECT_CALL(*_mock, deleteItem(createPath(NUMBER_TWO, PATH_INC_NUM)))
        .WillOnce(Return(true));
    _mobileClient->callEnd();
}

TEST_F(MobileClientTest, shouldFailedToCallEnd)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    _mobileClient->callEnd();
}

TEST_F(MobileClientTest, shouldFailedToCallEnd_1)
{
    mockRegisterClient(NAME, NUMBER);
    _mobileClient->setOutNUm(NUMBER_TWO);
    const pair<string, string> setData1 = make_pair(createPath(NUMBER_TWO, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData1)).Times(1);
    const pair<string, string> setData2 = make_pair(createPath(NUMBER, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData2)).Times(1);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, deleteItem(createPath(NUMBER_TWO, PATH_INC_NUM)))
        .WillOnce(Return(true));
    _mobileClient->callEnd();
}

TEST_F(MobileClientTest, shouldSuccessedToSetState)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER, PATH_STATE), ACTIVE);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    _mobileClient->setState(NUMBER, ACTIVE);
}

TEST_F(MobileClientTest, shouldSuccessedToReject)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER_TWO, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    const pair<string, string> setData1 = make_pair(createPath(NUMBER, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData1)).Times(1);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_INC_NUM), NUMBER_TWO);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getNumber(), PATH_INC_NUM)))
        .WillOnce(Return(true));
    _mobileClient->reject();
}

TEST_F(MobileClientTest, shouldSuccessedToRejectTrueOutNum)
{
    mockRegisterClient(NAME, NUMBER);
    const pair<string, string> setData = make_pair(createPath(NUMBER_TWO, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData)).Times(1);
    const pair<string, string> setData1 = make_pair(createPath(NUMBER, PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, changeData(setData1)).Times(1);
    _mobileClient->setOutNUm(NUMBER_TWO);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getOutNumber(), PATH_INC_NUM)))
        .WillOnce(Return(true));
    _mobileClient->reject();
}

TEST_F(MobileClientTest, shouldSuccesseddToUnregister)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getNumber())))
        .WillOnce(Return(true));
    _mobileClient->unregister();
}

TEST_F(MobileClientTest, shouldFailedToUnregister)
{
    mockRegisterClient(NAME, NUMBER);
    map<string, string> dataForOne;
    dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
    EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
        .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
    _mobileClient->unregister();
}

TEST_F(MobileClientTest, shouldSuccessedToGetName)
{
    mockRegisterClient(NAME, NUMBER);
    string tname = _mobileClient->getName();
    if (tname != NAME)
    {
        cout << "error, name does not match " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccessedToCreatePathWithoutKey)
{
    string path = _mobileClient->createPath(NUMBER);
    if (path != (PATH + NUMBER +"']"))
    {
        cout  << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccessedToCreatePathWithKey)
{
    string path = _mobileClient->createPath(NUMBER,PATH_INC_NUM);
    if (path != (PATH + NUMBER +"']" + PATH_INC_NUM))
    {
        cout  << " --inconsistency " << endl;
    }
}

TEST_F(MobileClientTest, shouldSuccessedToSetIsReg)
{
_mobileClient->setIsReg(true);
bool state = _mobileClient->getIsReg();
if (!state)
{
    cout  << " --inconsistency " << endl;
}
}

TEST_F(MobileClientTest, shouldSuccessedToGetIsReg)
{
_mobileClient->setIsReg(true);
bool state = _mobileClient->getIsReg();
if (!state)
{
    cout  << " --inconsistency " << endl;
}
}
