/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/wip/ad_targeting/processors/url_processor.h"

#include "bat/ads/internal/logging.h"

namespace ads {
namespace ad_targeting {
namespace processor {

namespace {
const uint16_t kPurchaseIntentWordCountLimit = 1000;
}  // namespace

Url::Url(
    const Resource& resource)
    : resource_(resource) {
}

Url::~Url() = default;

void Url::Process(
    const GURL& url) {
  if (!resource_.IsInitialized()) {
    BLOG(1, "Failed to process purchase intent signal for visited URL due to "
        "uninitialized purchase intent resource");

    return;
  }

  if (!url.is_valid()) {
    BLOG(1, "Failed to process purchase intent signal for visited URL due to "
        "an invalid url");

    return;
  }

  PurchaseIntentSignalInfo purchase_intent_signal =
      ExtractIntentSignal(url.value());

  if (purchase_intent_signal.segments.empty()) {
    BLOG(1, "No purchase intent matches found for visited URL");
    return;
  }

  BLOG(1, "Extracted purchase intent signal from visited URL");

  AppendIntentSignalToHistory(purchase_intent_signal);
}

///////////////////////////////////////////////////////////////////////////////

PurchaseIntentSignalInfo PurchaseIntentClassifier::ExtractIntentSignal(
    const std::string& url) {
  PurchaseIntentSignalInfo signal_info;
  const std::string search_query =
      SearchProviders::ExtractSearchQueryKeywords(url);

  if (!search_query.empty()) {
    auto keyword_segments = GetSegments(search_query);

    if (!keyword_segments.empty()) {
      uint16_t keyword_weight = GetFunnelWeight(search_query);

      signal_info.timestamp_in_seconds =
          static_cast<uint64_t>(base::Time::Now().ToDoubleT());
      signal_info.segments = keyword_segments;
      signal_info.weight = keyword_weight;
      return signal_info;
    }
  } else {
    SiteInfo info = GetSite(url);

    if (!info.url_netloc.empty()) {
      signal_info.timestamp_in_seconds =
          static_cast<uint64_t>(base::Time::Now().ToDoubleT());
      signal_info.segments = info.segments;
      signal_info.weight = info.weight;
      return signal_info;
    }
  }

  return signal_info;
}

void PurchaseIntentClassifier::AppendIntentSignalToHistory(
    const PurchaseIntentSignalInfo& purchase_intent_signal) {
  for (const auto& segment : purchase_intent_signal.segments) {
    PurchaseIntentSignalHistoryInfo history;
    history.timestamp_in_seconds = purchase_intent_signal.timestamp_in_seconds;
    history.weight = purchase_intent_signal.weight;
    Client::Get()->AppendToPurchaseIntentSignalHistoryForSegment(
        segment, history);
  }
}

SiteInfo PurchaseIntentClassifier::GetSite(
    const std::string& url) {
  const GURL visited_url = GURL(url);
  SiteInfo info;

  if (!visited_url.has_host()) {
    return info;
  }

  const resource::PurchaseIntentInfo purchase_intent_resource = resource_.get();
  for (const auto& site : purchase_intent_resource.sites) {
    const GURL site_url = GURL(site.url_netloc);

    if (!site_url.is_valid()) {
      continue;
    }

    if (SameDomainOrHost(visited_url, site_url,
        net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
      info = site;
      return info;
    }
  }

  return info;
}

PurchaseIntentSegmentList PurchaseIntentClassifier::GetSegments(
    const std::string& search_query) {
  PurchaseIntentSegmentList segment_list;
  auto search_query_keyword_set = TransformIntoSetOfWords(search_query);

  const resource::PurchaseIntentInfo purchase_intent = resource_.get();
  for (const auto& keyword : purchase_intent.segment_keywords) {
    auto list_keyword_set = TransformIntoSetOfWords(keyword.keywords);

    // Intended behavior relies on early return from list traversal and
    // implicitely on the ordering of |segment_keywords_| to ensure specific
    // segments are matched over general segments, e.g. "audi a6" segments
    // should be returned over "audi" segments if possible
    if (IsSubset(search_query_keyword_set, list_keyword_set)) {
      segment_list = keyword.segments;
      return segment_list;
    }
  }

  return segment_list;
}

uint16_t PurchaseIntentClassifier::GetFunnelWeight(
    const std::string& search_query) {
  auto search_query_keyword_set = TransformIntoSetOfWords(search_query);

  uint16_t max_weight = kPurchaseIntentDefaultSignalWeight;
  const resource::PurchaseIntentInfo purchase_resource = resource_.get();
  for (const auto& keyword : purchase_resource.funnel_keywords) {
    auto list_keyword_set = TransformIntoSetOfWords(keyword.keywords);

    if (IsSubset(search_query_keyword_set, list_keyword_set) &&
        keyword.weight > max_weight) {
      max_weight = keyword.weight;
    }
  }

  return max_weight;
}

// TODO(https://github.com/brave/brave-browser/issues/8495): Implement Brave
// Ads Purchase Intent keyword matching with std::sets
bool PurchaseIntentClassifier::IsSubset(
    const std::vector<std::string>& keyword_set_a,
    const std::vector<std::string>& keyword_set_b) {
  std::vector<std::string> sorted_keyword_set_a = keyword_set_a;
  std::sort(sorted_keyword_set_a.begin(), sorted_keyword_set_a.end());

  std::vector<std::string> sorted_keyword_set_b = keyword_set_b;
  std::sort(sorted_keyword_set_b.begin(), sorted_keyword_set_b.end());

  return std::includes(sorted_keyword_set_a.begin(), sorted_keyword_set_a.end(),
      sorted_keyword_set_b.begin(), sorted_keyword_set_b.end());
}

// TODO(https://github.com/brave/brave-browser/issues/8495): Implement Brave
// Ads Purchase Intent keyword matching with std::sets
std::vector<std::string> PurchaseIntentClassifier::TransformIntoSetOfWords(
    const std::string& text) {
  std::string lowercase_text = StripHtmlTagsAndNonAlphaNumericCharacters(text);
  std::transform(lowercase_text.begin(), lowercase_text.end(),
  lowercase_text.begin(), ::tolower);

  std::stringstream sstream(lowercase_text);
  std::vector<std::string> set_of_words;
  std::string word;
  uint16_t word_count = 0;
  while (sstream >> word && word_count < kPurchaseIntentWordCountLimit) {
    set_of_words.push_back(word);
    word_count++;
  }

  return set_of_words;
}

}  // namespace processor
}  // namespace ad_targeting
}  // namespace ads
