/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_SERVICES_BAT_ADS_BAT_ADS_IMPL_H_
#define BRAVE_COMPONENTS_SERVICES_BAT_ADS_BAT_ADS_IMPL_H_

#include <stdint.h>
#include <memory>
#include <string>
#include <utility>

#include "base/memory/weak_ptr.h"
#include "brave/components/services/bat_ads/public/interfaces/bat_ads.mojom.h"
#include "mojo/public/cpp/bindings/interface_request.h"
#include "bat/ads/ads.h"
#include "bat/ads/statement_info.h"

namespace ads {
class Ads;
}

namespace bat_ads {

class BatAdsClientMojoBridge;

class BatAdsImpl :
    public mojom::BatAds,
    public base::SupportsWeakPtr<BatAdsImpl> {
 public:
  explicit BatAdsImpl(
      mojo::PendingAssociatedRemote<mojom::BatAdsClient> client_info);
  ~BatAdsImpl() override;

  BatAdsImpl(const BatAdsImpl&) = delete;
  BatAdsImpl& operator=(const BatAdsImpl&) = delete;

  // Overridden from mojom::BatAds:
  void Initialize(
      InitializeCallback callback) override;
  void Shutdown(
      ShutdownCallback callback) override;

  void ChangeLocale(
      const std::string& locale) override;

  void OnAdsSubdivisionTargetingCodeHasChanged() override;

  void OnPageLoaded(
      const int32_t tab_id,
      const std::string& original_url,
      const std::string& url,
      const std::string& html) override;

  void OnUnIdle() override;
  void OnIdle() override;

  void OnForeground() override;
  void OnBackground() override;

  void OnMediaPlaying(
      const int32_t tab_id) override;
  void OnMediaStopped(
      const int32_t tab_id) override;

  void OnTabUpdated(
      const int32_t tab_id,
      const std::string& url,
      const bool is_active,
      const bool is_browser_active,
      const bool is_incognito) override;
  void OnTabClosed(
      const int32_t tab_id) override;

  void GetAdNotification(
      const std::string& uuid,
      GetAdNotificationCallback callback) override;
  void OnAdNotificationEvent(
      const std::string& uuid,
      const ads::AdNotificationEventType event_type) override;

  void OnNewTabPageAdEvent(
      const std::string& wallpaper_id,
      const std::string& creative_instance_id,
      const ads::NewTabPageAdEventType event_type) override;

  void RemoveAllHistory(
      RemoveAllHistoryCallback callback) override;

  void OnWalletUpdated(
      const std::string& payment_id,
      const std::string& seed) override;

  void ReconcileAdRewards() override;

  void GetAdsHistory(
      const int64_t from_timestamp,
      const int64_t to_timestamp,
      GetAdsHistoryCallback callback) override;

  void GetTransactionHistory(
      GetTransactionHistoryCallback callback) override;

  void ToggleAdThumbUp(
      const std::string& creative_instance_id,
      const std::string& creative_set_id,
      const int action,
      ToggleAdThumbUpCallback callback) override;
  void ToggleAdThumbDown(
      const std::string& creative_instance_id,
      const std::string& creative_set_id,
      const int action,
      ToggleAdThumbUpCallback callback) override;
  void ToggleAdOptInAction(
      const std::string& category,
      const int action,
      ToggleAdOptInActionCallback callback) override;
  void ToggleAdOptOutAction(
      const std::string& category,
      const int action,
      ToggleAdOptOutActionCallback callback) override;
  void ToggleSaveAd(
      const std::string& creative_instance_id,
      const std::string& creative_set_id,
      const bool saved,
      ToggleSaveAdCallback callback) override;
  void ToggleFlagAd(
      const std::string& creative_instance_id,
      const std::string& creative_set_id,
      const bool flagged,
      ToggleFlagAdCallback callback) override;

  void OnUserModelUpdated(
      const std::string& id) override;

 private:
  // Workaround to pass base::OnceCallback into std::bind
  template <typename Callback>
    class CallbackHolder {
     public:
      CallbackHolder(base::WeakPtr<BatAdsImpl> client, Callback callback) :
          client_(client),
          callback_(std::move(callback)) {}
      ~CallbackHolder() = default;

      bool is_valid() {
        return !!client_.get();
      }

      Callback& get() {
        return callback_;
      }

     private:
      base::WeakPtr<BatAdsImpl> client_;
      Callback callback_;
    };

  static void OnInitialize(
      CallbackHolder<InitializeCallback>* holder,
      const int32_t result);

  static void OnShutdown(
      CallbackHolder<ShutdownCallback>* holder,
      const int32_t result);

  static void OnRemoveAllHistory(
      CallbackHolder<RemoveAllHistoryCallback>* holder,
      const int32_t result);

  static void OnGetTransactionHistory(
    CallbackHolder<GetTransactionHistoryCallback>* holder,
    const bool success,
    const ads::StatementInfo& statement);

  std::unique_ptr<BatAdsClientMojoBridge> bat_ads_client_mojo_proxy_;
  std::unique_ptr<ads::Ads> ads_;
};

}  // namespace bat_ads

#endif  // BRAVE_COMPONENTS_SERVICES_BAT_ADS_BAT_ADS_IMPL_H_
