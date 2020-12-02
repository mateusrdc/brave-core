/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/content_settings/core/browser/brave_content_settings_utils.h"

#include <algorithm>

#include "base/optional.h"
#include "url/gurl.h"

namespace {

const std::vector<ContentSettingsType> kShieldsContentSettingsTypes {
    ContentSettingsType::BRAVE_ADS,
    ContentSettingsType::BRAVE_COSMETIC_FILTERING,
    ContentSettingsType::BRAVE_TRACKERS,
    ContentSettingsType::BRAVE_HTTP_UPGRADABLE_RESOURCES,
    ContentSettingsType::BRAVE_JAVASCRIPT,
    ContentSettingsType::BRAVE_FINGERPRINTING_V2,
    ContentSettingsType::BRAVE_SHIELDS,
    ContentSettingsType::BRAVE_REFERRERS,
    ContentSettingsType::BRAVE_COOKIES,
    ContentSettingsType::BRAVE_FACEBOOK_EMBEDS,
    ContentSettingsType::BRAVE_TWITTER_EMBEDS,
    ContentSettingsType::BRAVE_LINKEDIN_EMBEDS
};

bool CanPatternBeConvertedToWildcardSchemeAndPort(
    const ContentSettingsPattern& pattern) {
  // 1. Wildcard is already in the desired state.
  // 2. Our firstParty placeholder shouldn't be converted.
  // 3. Patterns that have file:// scheme.
  // 4. We only want to convert patterns that have a specific host, so something
  // like "http://*:80/*" should be left alone.
  if (pattern == ContentSettingsPattern::Wildcard() ||
      pattern == ContentSettingsPattern::FromString("https://firstParty/*") ||
      pattern.GetScheme() == ContentSettingsPattern::SCHEME_FILE ||
      pattern.MatchesAllHosts() || pattern.GetHost().empty())
    return false;
  // Check for the case when the scheme is wildcard, but the port isn't.
  if (pattern.GetScheme() == ContentSettingsPattern::SCHEME_WILDCARD) {
    GURL check_for_port_url("http://" + pattern.ToString());
    return check_for_port_url.has_port();
  }
  GURL url(pattern.ToString());
  if (!url.is_valid() || url.is_empty() || !url.has_host())
    return false;
  if (url.has_scheme())
    return !ContentSettingsPattern::IsNonWildcardDomainNonPortScheme(
        url.scheme_piece());
  return url.has_port();
}

}  // namespace

namespace content_settings {

const std::vector<ContentSettingsType>& GetShieldsContentSettingsTypes() {
  return kShieldsContentSettingsTypes;
}

bool IsShieldsContentSettingsType(
    const ContentSettingsType& content_type) {
  return std::find(kShieldsContentSettingsTypes.begin(),
                   kShieldsContentSettingsTypes.end(),
                   content_type) != kShieldsContentSettingsTypes.end();
}

base::Optional<ContentSettingsPattern> ConvertPatternToWildcardSchemeAndPort(
    const ContentSettingsPattern& pattern) {
  if (!CanPatternBeConvertedToWildcardSchemeAndPort(pattern))
    return base::nullopt;
  DCHECK(!pattern.GetHost().empty());
  base::Optional<ContentSettingsPattern> new_pattern =
      ContentSettingsPattern::FromString("*://" + pattern.GetHost() + "/*");
  return new_pattern;
}

}  // namespace content_settings
