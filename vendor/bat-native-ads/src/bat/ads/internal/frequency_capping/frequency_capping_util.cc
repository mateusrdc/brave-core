/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/frequency_capping_util.h"

#include "bat/ads/internal/time_util.h"

namespace ads {

std::deque<int64_t> GetTimestampHistoryForAdEvents(
    const AdEventList& ad_events) {
  std::deque<int64_t> history;

  for (const auto& ad_event : ad_events) {
    history.push_back(ad_event.timestamp);
  }

  return history;
}

bool DoesHistoryRespectCapForRollingTimeConstraint(
    const std::deque<int64_t> history,
    const int64_t time_constraint_in_seconds,
    const uint64_t cap) {
  uint64_t count = 0;

  const int64_t now_in_seconds =
      static_cast<int64_t>(base::Time::Now().ToDoubleT());

  for (const auto& timestamp_in_seconds : history) {
    if (now_in_seconds - timestamp_in_seconds < time_constraint_in_seconds) {
      count++;
    }
  }

  if (count >= cap) {
    return false;
  }

  return true;
}

uint64_t OccurrencesForRollingTimeConstraint(
    const std::deque<int64_t> history,
    const int64_t time_constraint_in_seconds) {
  uint64_t count = 0;

  const int64_t now_in_seconds =
      static_cast<int64_t>(base::Time::Now().ToDoubleT());

  for (const auto& timestamp_in_seconds : history) {
    if (now_in_seconds - timestamp_in_seconds < time_constraint_in_seconds) {
      count++;
    }
  }

  return count;
}

}  // namespace ads
