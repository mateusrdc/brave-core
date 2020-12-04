/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_WIP_AD_TARGETING_PROCESSORS_AD_PROCESSOR_H_
#define BAT_ADS_INTERNAL_WIP_AD_TARGETING_PROCESSORS_AD_PROCESSOR_H_

#include "bat/ads/internal/wip/ad_targeting/processor.h"

namespace ads {

class AdInfo;

namespace ad_targeting {
namespace processor {

class Ad : public Processor<AdInfo> {
 public:
  Ad();

  ~Ad() override;

  void Process(
      const AdInfo& ad) override;
};

}  // namespace processor
}  // namespace ad_targeting
}  // namespace ads

#endif  // BAT_ADS_INTERNAL_WIP_AD_TARGETING_PROCESSORS_AD_PROCESSOR_H_
