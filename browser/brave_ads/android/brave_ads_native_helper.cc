/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_ads/android/brave_ads_native_helper.h"

#include <memory>
#include <string>

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "brave/browser/brave_ads/android/jni_headers/BraveAdsNativeHelper_jni.h"
#include "brave/browser/brave_rewards/rewards_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_android.h"

using base::android::JavaParamRef;
using base::android::ScopedJavaLocalRef;

namespace brave_ads {

class AdsNotificationhandler;
class AdsService;
class AdsServiceFactory;
class AdsServiceImpl;

// static

jboolean JNI_BraveAdsNativeHelper_IsBraveAdsEnabled(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  auto* ads_service_ = brave_ads::AdsServiceFactory::GetForProfile(profile);
  if (!ads_service_) {
    NOTREACHED();
    return false;
  }

  return ads_service_->IsEnabled();
}

jboolean JNI_BraveAdsNativeHelper_IsLocaleValid(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  auto* ads_service_ = brave_ads::AdsServiceFactory::GetForProfile(profile);
  if (!ads_service_) {
    NOTREACHED();
    return false;
  }

  return ads_service_->IsSupportedLocale();
}

jboolean JNI_BraveAdsNativeHelper_IsSupportedLocale(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  auto* ads_service_ = brave_ads::AdsServiceFactory::GetForProfile(profile);
  if (!ads_service_) {
    NOTREACHED();
    return false;
  }

  return ads_service_->IsSupportedLocale();
}

jboolean JNI_BraveAdsNativeHelper_IsNewlySupportedLocale(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  auto* ads_service_ = brave_ads::AdsServiceFactory::GetForProfile(profile);
  if (!ads_service_) {
    NOTREACHED();
    return false;
  }

  return ads_service_->IsNewlySupportedLocale();
}

void JNI_BraveAdsNativeHelper_SetAdsEnabled(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  auto* rewards_service =
      brave_rewards::RewardsServiceFactory::GetForProfile(profile);
  if (!rewards_service) {
    NOTREACHED();
    return;
  }

  rewards_service->SetAdsEnabled(true);
}

void JNI_BraveAdsNativeHelper_AdNotificationClicked(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android,
    const base::android::JavaParamRef<jstring>& j_notification_id) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  brave_ads::AdsServiceImpl* ads_service = brave_ads::
      AdsServiceFactory::GetImplForProfile(profile);
  if (!ads_service) {
    NOTREACHED();
    return;
  }
  std::string notification_id =
      base::android::ConvertJavaStringToUTF8(env, j_notification_id);
  std::unique_ptr<brave_ads::AdsNotificationHandler> handler =
      std::make_unique<brave_ads::AdsNotificationHandler>(
          static_cast<content::BrowserContext*>(profile));
  handler->SetAdsService(ads_service);
  handler->OnClick(profile,
      GURL(""),
      notification_id,
      base::nullopt,
      base::nullopt,
      base::OnceClosure());
}

void JNI_BraveAdsNativeHelper_AdNotificationDismissed(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile_android,
    const base::android::JavaParamRef<jstring>& j_notification_id,
    jboolean j_by_user) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile_android);
  brave_ads::AdsServiceImpl* ads_service =
        brave_ads::AdsServiceFactory::GetImplForProfile(profile);
  if (!ads_service) {
    NOTREACHED();
    return;
  }
  std::string notification_id =
      base::android::ConvertJavaStringToUTF8(env, j_notification_id);
  std::unique_ptr<brave_ads::AdsNotificationHandler> handler =
      std::make_unique<brave_ads::AdsNotificationHandler>(
          static_cast<content::BrowserContext*>(profile));
  handler->SetAdsService(ads_service);
  handler->OnClose(profile,
      GURL(""),
      notification_id,
      j_by_user,
      base::OnceClosure());
}

}  // namespace brave_ads
