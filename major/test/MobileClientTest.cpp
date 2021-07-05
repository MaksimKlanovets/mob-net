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
    const string MODULE_NAME = "mobile-network";
    const string BUSY = "busy";
    const string ACTIVE = "active";
    const string IDLE = "idle";
    const string NAME = "Max";
    const string NUMBER = "001";
    const string NAME_TWO = "MAT";
    const string NUMBER_TWO = "002";
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
/////////////////done
// TEST_F(MobileClientTest, shouldSuccedRegisterCleint)
// {
//     mockRegisterClient(NAME, NUMBER);
// }

// TEST_F(MobileClientTest, shouldSuccedToHandleOperData)
// {
//     _mobileClient->setName(NAME);
//     string tname = "testName";
//     _mobileClient->handleOperData(tname);
//     if (NAME != tname)
//     {
//         cout << "incName " << NAME << " outgoing " << tname << " --inconsistency " << endl;
//     }
// }

// TEST_F(MobileClientTest, shouldSuccedToSetName)
// {
//     _mobileClient->setName(NAME);
//     string tName = _mobileClient->getName();
//     if (NAME != tName)
//     {
//         cout << "incName->" << NAME << ";\toutgoing->" << tName << " --inconsistency " << endl;
//     }
// }

// TEST_F(MobileClientTest, shouldSuccedToSetNumber)
// {
//     _mobileClient->setNumber(NUMBER);
//     string tNumber = _mobileClient->getNumber();
//     if (NUMBER != tNumber)
//     {
//         cout << "incNumber->" << NUMBER << ";\toutgoing->" << tNumber << " --inconsistency " << endl;
//     }
// }

// TEST_F(MobileClientTest, shouldSuccedToSetState)
// {
//     EXPECT_CALL(*_mock, changeData(_)).Times(1);
//     _mobileClient->setState(NUMBER, ACTIVE);
// }
///DONE
// TEST_F(MobileClientTest, shouldSuccedToMakeCallTrue)
// {
//     mockRegisterClient(NAME, NUMBER);
//     EXPECT_CALL(*_mock, changeData(_)).Times(3);
//     map<string, string> dataForOne;
//     map<string, string> dataForTwo;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
//     dataForTwo.emplace(createPath(NUMBER_TWO, PATH_STATE), IDLE);
//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
//     EXPECT_CALL(*_mock, fetchData(createPath(NUMBER_TWO), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForTwo), Return(true)));
//     _mobileClient->makeCall(NUMBER_TWO);
// }
////////////done
// TEST_F(MobileClientTest, shouldSuccedToAnswerTrue)
// {
//     mockRegisterClient(NAME, NUMBER);
//     EXPECT_CALL(*_mock, changeData(_)).Times(2);
//     map<string, string> dataForOne;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_INC_NUM), NUMBER_TWO);
//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber(),PATH_INC_NUM), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
//     _mobileClient->answer();
// }
///////////////DONE
// TEST_F(MobileClientTest, shouldSuccedToCallEndTrue)
// {
//     mockRegisterClient(NAME, NUMBER);
//     EXPECT_CALL(*_mock, changeData(_)).Times(3);
//     map<string, string> dataForOne;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_INC_NUM), NUMBER_TWO);
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), ACTIVE);
//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
//     EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getNumber(),PATH_INC_NUM)))
//         .WillOnce(Return(true));
//     EXPECT_CALL(*_mock, deleteItem(createPath(NUMBER_TWO,PATH_INC_NUM)))
//         .WillOnce(Return(true));
//     _mobileClient->callEnd();
// }
///////DONE
// TEST_F(MobileClientTest, shouldSuccedToRejectTrue)
// {
//     mockRegisterClient(NAME, NUMBER);
//     EXPECT_CALL(*_mock, changeData(_)).Times(2);

//     map<string, string> dataForOne;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_INC_NUM), NUMBER_TWO);

//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
//     EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getNumber(), PATH_INC_NUM)))
//         .WillOnce(Return(true));
//     _mobileClient->reject();
// }
//done
// TEST_F(MobileClientTest, shouldSuccedToRejectTrueOutNum)
// {
//     mockRegisterClient(NAME, NUMBER);
//     EXPECT_CALL(*_mock, changeData(_)).Times(2);
//     _mobileClient->setOutNUm(NUMBER_TWO);
//     map<string, string> dataForOne;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);

//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
//     EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getOutNumber(), PATH_INC_NUM)))
//         .WillOnce(Return(true));
//     _mobileClient->reject();
// }
//done
// TEST_F(MobileClientTest, shouldSuccedToUnregisterTrue)
// {
//     mockRegisterClient(NAME, NUMBER);
//     map<string, string> dataForOne;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), IDLE);
//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));
//     EXPECT_CALL(*_mock, deleteItem(createPath(_mobileClient->getNumber())))
//         .WillOnce(Return(true));

//     _mobileClient->unregister();
// }
//done
// TEST_F(MobileClientTest, shouldSuccedToUnregisterFalse)
// {
//     mockRegisterClient(NAME, NUMBER);
//     map<string, string> dataForOne;
//     dataForOne.emplace(createPath(_mobileClient->getNumber(), PATH_STATE), BUSY);
//     EXPECT_CALL(*_mock, fetchData(createPath(_mobileClient->getNumber()), _))
//         .WillOnce(DoAll(SetArgReferee<1>(dataForOne), Return(true)));

//     _mobileClient->unregister();
// }

// TEST_F(MobileClientTest, shouldSuccedToGetName)
// {
//     _mobileClient->setName(NAME);
//     string tname = _mobileClient->getName();
//     if (tname != NAME)
//     {
//         cout << "error, name does not match " << endl;
//     }
// }

// TEST_F(MobileClientTest, shouldSuccedToCreatePath)
// {
//     string path = _mobileClient->createPath(NUMBER);
//     if (path != (PATH + NUMBER +"']"))
//     {
//         cout  << " --inconsistency " << endl;
//     }
// }

// TEST_F(MobileClientTest, shouldSuccedToSetIsReg)
// {
// _mobileClient->setIsReg(true);
// bool state = _mobileClient->getIsReg();
// if (!state)
// {
//     cout  << " --inconsistency " << endl;
// }
// }

// TEST_F(MobileClientTest, shouldSuccedToGetIsReg)
// {
// _mobileClient->setIsReg(true);
// bool state = _mobileClient->getIsReg();
// if (!state)
// {
//     cout  << " --inconsistency " << endl;
// }
// }
