/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_user_agent_network_delegate_helper.h"

#include <optional>
#include <string>

#include "base/containers/contains.h"
#include "base/test/scoped_feature_list.h"
#include "brave/components/brave_user_agent/browser/brave_user_agent_exceptions.h"
#include "brave/components/brave_user_agent/common/features.h"
#include "net/base/net_errors.h"
#include "net/http/http_request_headers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

constexpr std::string_view kSecCHUAHeader = "Sec-CH-UA";
constexpr std::string_view kSecCHUABrave =
    "\"Chromium\";v=\"140\", \"Brave\";v=\"140\", \"NotABrand\";v=\"99\"";
constexpr std::string_view kSecCHUAGoogleChrome =
    "\"Chromium\";v=\"140\", \"Google Chrome\";v=\"140\", "
    "\"NotABrand\";v=\"99\"";
constexpr std::string_view kSecCHUAMock = "Sec-CH-Mock";

namespace brave {

struct UserAgentTestResult {
  int result;
  std::optional<std::string> header_value;
};

class BraveUserAgentNetworkDelegateHelperTest : public ::testing::Test {
 protected:
  void SetUp() override {
    auto* exceptions =
        brave_user_agent::BraveUserAgentExceptions::GetInstance();
    exceptions->AddToExceptedDomainsForTesting("excepted.com");
    exceptions->SetIsReadyForTesting();
  }

  UserAgentTestResult RunUserAgentTest(bool feature_enabled,
                                       const std::string& tab_origin,
                                       const std::string& header_name,
                                       const std::string& header_value) {
    base::test::ScopedFeatureList feature_list;
    if (feature_enabled) {
      feature_list.InitAndEnableFeature(
          brave_user_agent::features::kUseBraveUserAgent);
    } else {
      feature_list.InitAndDisableFeature(
          brave_user_agent::features::kUseBraveUserAgent);
    }
    net::HttpRequestHeaders headers;
    headers.SetHeader(header_name, header_value);
    auto ctx = std::make_shared<BraveRequestInfo>();
    ctx->tab_origin = GURL(tab_origin);
    int result = OnBeforeStartTransaction_UserAgentWork(&headers, {}, ctx);
    EXPECT_EQ(result, net::OK);
    return {result, headers.GetHeader(kSecCHUAHeader)};
  }
};

TEST_F(BraveUserAgentNetworkDelegateHelperTest,
       ReplacesBraveWithGoogleChromeIfExcepted) {
  auto res = RunUserAgentTest(
      /*feature_enabled=*/true, "https://excepted.com",
      std::string(kSecCHUAHeader), std::string(kSecCHUABrave));
  ASSERT_TRUE(res.header_value.has_value());
  EXPECT_EQ(res.header_value.value(), kSecCHUAGoogleChrome);
}

TEST_F(BraveUserAgentNetworkDelegateHelperTest,
       DoesNotReplaceBraveWithGoogleChromeIfNotExcepted) {
  auto res = RunUserAgentTest(
      /*feature_enabled=*/true, "https://not-excepted.com",
      std::string(kSecCHUAHeader), std::string(kSecCHUABrave));
  ASSERT_TRUE(res.header_value.has_value());
  EXPECT_EQ(res.header_value.value(), kSecCHUABrave);
}

TEST_F(BraveUserAgentNetworkDelegateHelperTest,
       DoesNotReplaceBraveWithGoogleChromeIfHeaderNotSet) {
  auto res = RunUserAgentTest(
      /*feature_enabled=*/true, "https://excepted.com",
      std::string(kSecCHUAMock), std::string(kSecCHUABrave));
  EXPECT_FALSE(res.header_value.has_value());
}

TEST_F(BraveUserAgentNetworkDelegateHelperTest,
       DoesNotReplaceBraveWithGoogleChromeIfHeaderSetToEmpty) {
  auto res = RunUserAgentTest(
      /*feature_enabled=*/true, "https://excepted.com",
      std::string(kSecCHUAHeader), "");
  ASSERT_TRUE(res.header_value.has_value());
  EXPECT_EQ(res.header_value.value(), "");
  EXPECT_FALSE(base::Contains(res.header_value.value(), "\"Brave\""));
  EXPECT_FALSE(base::Contains(res.header_value.value(), "\"Google Chrome\""));
}

TEST_F(BraveUserAgentNetworkDelegateHelperTest,
       DoesNotReplaceBraveWithGoogleChromeIfFeatureIsDisabled) {
  auto res = RunUserAgentTest(
      /*feature_enabled=*/false, "https://excepted.com",
      std::string(kSecCHUAHeader), std::string(kSecCHUABrave));
  ASSERT_TRUE(res.header_value.has_value());
  EXPECT_EQ(res.header_value.value(), kSecCHUABrave);
}

}  // namespace brave
