/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/ipfs_redirect_network_delegate_helper.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/test/scoped_feature_list.h"
#include "brave/browser/net/url_context.h"
#include "brave/common/translate_network_constants.h"
#include "brave/components/ipfs/features.h"
#include "brave/components/ipfs/ipfs_constants.h"
#include "brave/components/ipfs/ipfs_gateway.h"
#include "brave/components/ipfs/ipfs_utils.h"
#include "brave/components/ipfs/pref_names.h"
#include "chrome/common/channel_info.h"
#include "chrome/test/base/chrome_render_view_host_test_harness.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/testing_pref_service.h"
#include "components/user_prefs/user_prefs.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_browser_context.h"
#include "net/traffic_annotation/network_traffic_annotation_test_helper.h"
#include "net/url_request/url_request_test_util.h"
#include "url/gurl.h"

namespace {

GURL GetLocalGateway() {
  return GURL("http://localhost:48080");
}

GURL GetPublicGateway() {
  return GURL("https://dweb.link");
}

const char initiator_cid[] =
    "bafybeiemxf5abjwjbikoz4mc3a3dla6ual3jsgpdr4cjr3oz3evfyavhwq";

}  // namespace

namespace ipfs {

class IPFSRedirectNetworkDelegateHelperTest : public testing::Test {
 public:
  IPFSRedirectNetworkDelegateHelperTest()
      : browser_context_(new content::TestBrowserContext()) {}
  ~IPFSRedirectNetworkDelegateHelperTest() override = default;

  void SetUp() override {
    feature_list_.InitAndEnableFeature(ipfs::features::kIpfsFeature);
    prefs_.registry()->RegisterIntegerPref(
        kIPFSResolveMethod, static_cast<int>(IPFSResolveMethodTypes::IPFS_ASK));
    user_prefs::UserPrefs::Set(browser_context_.get(), &prefs_);
  }

  content::TestBrowserContext* browser_context() {
    return browser_context_.get();
  }

 private:
  content::BrowserTaskEnvironment task_environment_;
  std::unique_ptr<content::TestBrowserContext> browser_context_;
  TestingPrefServiceSimple prefs_;
  base::test::ScopedFeatureList feature_list_;
};

TEST_F(IPFSRedirectNetworkDelegateHelperTest, TranslateIPFSURIHTTPScheme) {
  GURL url("http://a.com/ipfs/QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG");
  auto brave_request_info = std::make_shared<brave::BraveRequestInfo>(url);
  brave_request_info->browser_context = browser_context();
  int rc = ipfs::OnBeforeURLRequest_IPFSRedirectWork(brave::ResponseCallback(),
                                                     brave_request_info);
  EXPECT_EQ(rc, net::OK);
  EXPECT_TRUE(brave_request_info->new_url_spec.empty());
}

TEST_F(IPFSRedirectNetworkDelegateHelperTest, TranslateIPFSURIIPFSSchemeLocal) {
  GURL url("ipfs://QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG");
  auto brave_request_info = std::make_shared<brave::BraveRequestInfo>(url);
  brave_request_info->browser_context = browser_context();
  brave_request_info->ipfs_gateway_url = GetLocalGateway();
  brave_request_info->initiator_url = ipfs::GetIPFSGatewayURL(
      initiator_cid, "",
      ipfs::GetDefaultIPFSLocalGateway(chrome::GetChannel()));
  int rc = ipfs::OnBeforeURLRequest_IPFSRedirectWork(brave::ResponseCallback(),
                                                     brave_request_info);
  EXPECT_EQ(rc, net::OK);
  EXPECT_EQ(brave_request_info->new_url_spec,
            "http://localhost:48080/ipfs/"
            "QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG");
}

TEST_F(IPFSRedirectNetworkDelegateHelperTest, TranslateIPFSURIIPFSScheme) {
  GURL url("ipfs://QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG");
  auto brave_request_info = std::make_shared<brave::BraveRequestInfo>(url);
  brave_request_info->browser_context = browser_context();
  brave_request_info->ipfs_gateway_url = GetPublicGateway();
  brave_request_info->initiator_url =
      ipfs::GetIPFSGatewayURL(initiator_cid, "", ipfs::GetDefaultIPFSGateway());
  int rc = ipfs::OnBeforeURLRequest_IPFSRedirectWork(brave::ResponseCallback(),
                                                     brave_request_info);
  EXPECT_EQ(rc, net::OK);
  EXPECT_EQ(
      brave_request_info->new_url_spec,
      "https://dweb.link/ipfs/QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG");
}

TEST_F(IPFSRedirectNetworkDelegateHelperTest, TranslateIPFSURIIPNSSchemeLocal) {
  GURL url("ipns://QmSrPmbaUKA3ZodhzPWZnpFgcPMFWF4QsxXbkWfEptTBJd");
  auto brave_request_info = std::make_shared<brave::BraveRequestInfo>(url);
  brave_request_info->browser_context = browser_context();
  brave_request_info->ipfs_gateway_url = GetLocalGateway();
  brave_request_info->initiator_url = ipfs::GetIPFSGatewayURL(
      initiator_cid, "",
      ipfs::GetDefaultIPFSLocalGateway(chrome::GetChannel()));
  int rc = ipfs::OnBeforeURLRequest_IPFSRedirectWork(brave::ResponseCallback(),
                                                     brave_request_info);
  EXPECT_EQ(rc, net::OK);
  EXPECT_EQ(brave_request_info->new_url_spec,
            "http://localhost:48080/ipns/"
            "QmSrPmbaUKA3ZodhzPWZnpFgcPMFWF4QsxXbkWfEptTBJd");
}

TEST_F(IPFSRedirectNetworkDelegateHelperTest, TranslateIPFSURIIPNSScheme) {
  GURL url("ipns://QmSrPmbaUKA3ZodhzPWZnpFgcPMFWF4QsxXbkWfEptTBJd");
  auto brave_request_info = std::make_shared<brave::BraveRequestInfo>(url);
  brave_request_info->browser_context = browser_context();
  brave_request_info->ipfs_gateway_url = GetPublicGateway();
  brave_request_info->initiator_url =
      ipfs::GetIPFSGatewayURL(initiator_cid, "", ipfs::GetDefaultIPFSGateway());
  int rc = ipfs::OnBeforeURLRequest_IPFSRedirectWork(brave::ResponseCallback(),
                                                     brave_request_info);
  EXPECT_EQ(rc, net::OK);
  EXPECT_EQ(
      brave_request_info->new_url_spec,
      "https://dweb.link/ipns/QmSrPmbaUKA3ZodhzPWZnpFgcPMFWF4QsxXbkWfEptTBJd");
}

}  // namespace ipfs
