#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "MockMobileClient.hpp"

class MobileClientTest : public testing::Test
{
protected:
void SetUp() override
{
_mobileClient = std::make_unique<MobileClient>();
}

protected:
std::unique_ptr<nsMobileClient::MobileClient> _mobileClient;
testing::StrictMock<mock::MyMock> _mock;
};

// TEST_F(MobileClientTest, shouldSuccedToCall)
// {

// _mobileClient->call();
// }