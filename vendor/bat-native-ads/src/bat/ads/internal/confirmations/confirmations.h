/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_CONFIRMATIONS_CONFIRMATIONS_H_
#define BAT_ADS_INTERNAL_CONFIRMATIONS_CONFIRMATIONS_H_

#include <memory>
#include <string>

#include "bat/ads/ads.h"
#include "bat/ads/internal/confirmations/confirmations_observer.h"
#include "bat/ads/internal/privacy/tokens/token_generator_interface.h"
#include "bat/ads/internal/timer.h"
#include "bat/ads/internal/tokens/redeem_unblinded_token/redeem_unblinded_token_delegate.h"
#include "bat/ads/result.h"

namespace ads {

class AdRewards;
class ConfirmationsState;
class RedeemUnblindedToken;
struct CatalogIssuersInfo;
struct UnblindedTokenInfo;

class Confirmations
    : public RedeemUnblindedTokenDelegate {
 public:
  Confirmations(
      privacy::TokenGeneratorInterface* token_generator);

  ~Confirmations() override;

  // TODO(https://github.com/brave/brave-browser/issues/12563): Decouple Brave
  // Ads rewards state from confirmations
  void set_ad_rewards(
      AdRewards* ad_rewards);

  void AddObserver(
      ConfirmationsObserver* observer);
  void RemoveObserver(
      ConfirmationsObserver* observer);

  void SetCatalogIssuers(
      const CatalogIssuersInfo& catalog_issuers);

  void ConfirmAd(
      const std::string& creative_instance_id,
      const ConfirmationType& confirmation_type);

  void RetryAfterDelay();

 private:
  base::ObserverList<ConfirmationsObserver> observers_;

  privacy::TokenGeneratorInterface* token_generator_;  // NOT OWNED

  std::unique_ptr<ConfirmationsState> confirmations_state_;
  std::unique_ptr<RedeemUnblindedToken> redeem_unblinded_token_;

  ConfirmationInfo CreateConfirmation(
      const std::string& creative_instance_id,
      const ConfirmationType& confirmation_type) const;

  Timer retry_timer_;
  void CreateNewConfirmationAndAppendToRetryQueue(
      const ConfirmationInfo& confirmation);
  void AppendToRetryQueue(
      const ConfirmationInfo& confirmation);
  void RemoveFromRetryQueue(
      const ConfirmationInfo& confirmation);
  void Retry();

  void NotifyConfirmAd(
      const double estimated_redemption_value,
      const ConfirmationInfo& confirmation);

  void NotifyConfirmAdFailed(
      const ConfirmationInfo& confirmation);

  // RedeemUnblindedTokenDelegate implementation
  void OnDidRedeemUnblindedToken(
      const ConfirmationInfo& confirmation,
      const privacy::UnblindedTokenInfo& unblinded_payment_token) override;

  void OnFailedToRedeemUnblindedToken(
      const ConfirmationInfo& confirmation,
      const bool should_retry) override;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_CONFIRMATIONS_CONFIRMATIONS_H_
