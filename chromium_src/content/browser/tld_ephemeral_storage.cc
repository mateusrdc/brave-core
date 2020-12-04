/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/chromium_src/content/public/browser/tld_ephemeral_storage.h"

#include <map>
#include "base/no_destructor.h"
#include "services/network/public/mojom/cookie_manager.mojom.h"

namespace content {

namespace {

using TLDEphemeralStorageMap =
    std::map<TLDEphemeralStorageKey, base::WeakPtr<TLDEphemeralStorage>>;

// This map allows TLDEphemeralStorage to manage the lifetime of ephemeral
// storage.
TLDEphemeralStorageMap& active_tld_storage_areas() {
  static base::NoDestructor<TLDEphemeralStorageMap> active_storage_areas;
  return *active_storage_areas.get();
}

}  // namespace

TLDEphemeralStorage::TLDEphemeralStorage(TLDEphemeralStorageKey key,
                                         StoragePartition* storage_partition)
    : key_(key), storage_partition_(storage_partition) {
  DCHECK(active_tld_storage_areas().find(key) ==
         active_tld_storage_areas().end());
  active_tld_storage_areas().emplace(key, weak_factory_.GetWeakPtr());
}

TLDEphemeralStorage::~TLDEphemeralStorage() {
  auto filter = network::mojom::CookieDeletionFilter::New();
  filter->ephemeral_storage_domain = key_.second;
  storage_partition_->GetCookieManagerForBrowserProcess()->DeleteCookies(
      std::move(filter), base::NullCallback());

  active_tld_storage_areas().erase(key_);
}

TLDEphemeralStorage* TLDEphemeralStorage::Get(BrowserContext* browser_context,
                                              std::string storage_domain) {
  TLDEphemeralStorageKey key = std::make_pair(browser_context, storage_domain);
  auto it = active_tld_storage_areas().find(key);
  DCHECK(it == active_tld_storage_areas().end() || it->second.get());
  return it != active_tld_storage_areas().end() ? it->second.get() : nullptr;
}

scoped_refptr<TLDEphemeralStorage> TLDEphemeralStorage::GetOrCreate(
    BrowserContext* browser_context,
    StoragePartition* storage_partition,
    std::string storage_domain) {
  if (TLDEphemeralStorage* existing = Get(browser_context, storage_domain)) {
    return existing;
  }

  TLDEphemeralStorageKey key = std::make_pair(browser_context, storage_domain);
  return base::MakeRefCounted<TLDEphemeralStorage>(key, storage_partition);
}

}  // namespace content
