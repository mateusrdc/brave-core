/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_SIDEBAR_SIDEBAR_MODEL_H_
#define BRAVE_BROWSER_UI_SIDEBAR_SIDEBAR_MODEL_H_

namespace sidebar {

// This model manage have sidebar items in runtime.
class SidebarModel {
 public:
  SidebarModel();
  virtual ~SidebarModel();

  SidebarModel(const SidebarModel&) = delete;
  SidebarModel& operator=(const SidebarModel&) = delete;
};

}  // sidebar

#endif  // BRAVE_BROWSER_UI_SIDEBAR_SIDEBAR_MODEL_H_
