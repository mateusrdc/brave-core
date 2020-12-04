/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/wip/ad_serving/ad_targeting/models/purchase_intent_model.h"

#include <stdint.h>

#include <map>
#include <string>

namespace ads {
namespace ad_targeting {
namespace model {

namespace {
const uint16_t kExpectedPurchaseIntentModelVersion = 1;
const uint16_t kPurchaseIntentDefaultSignalWeight = 1;
}  // namespace

PurchaseIntent::PurchaseIntent(
    const resource::PurchaseIntent& purchase_intent_resource)
    : purchase_intent_resource_(purchase_intent_resource) {
}

PurchaseIntent::~PurchaseIntent() = default;

SegmentList PurchaseIntent::GetSegments() const {
  if (history.empty()) {
    return {};
  }

  std::multimap<uint16_t, std::string> scores;
  for (const auto& segment_history : history) {
    uint16_t score = GetIntentScoreForHistory(segment_history.second);
    scores.insert(std::make_pair(score, segment_history.first));
  }

  SegmentList segments;
  std::multimap<uint16_t, std::string>::reverse_iterator rit;
  const resource::PurchaseIntentInfo purchase_intent =
      purchase_intent_resource_.get();

  for (rit=scores.rbegin(); rit != scores.rend(); ++rit) {
    if (rit->first >= purchase_intent.classification_threshold) {
      segments.push_back(rit->second);
    }

    if (segments.size() >= max_segments) {
      return segments;
    }
  }

  return segments;
}

uint16_t PurchaseIntentClassifier::GetIntentScoreForHistory(
    const PurchaseIntentSignalSegmentHistoryList& history) {
  uint16_t intent_score = 0;

  for (const auto& signal_segment : history) {
    const base::Time signal_decayed_at_in_seconds =
        base::Time::FromDoubleT(signal_segment.timestamp_in_seconds) +
            base::TimeDelta::FromSeconds(signal_decay_time_window_in_seconds_);

    const base::Time now_in_seconds = base::Time::Now();

    if (now_in_seconds > signal_decayed_at_in_seconds) {
      continue;
    }

    intent_score += signal_level_ * signal_segment.weight;
  }

  return intent_score;
}

}  // namespace model
}  // namespace ad_targeting
}  // namespace ads
