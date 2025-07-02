/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_ORIGIN_BRAVE_ORIGIN_PROFILE_OBSERVER_H_
#define BRAVE_BROWSER_BRAVE_ORIGIN_BRAVE_ORIGIN_PROFILE_OBSERVER_H_

#include "base/scoped_observation.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/profiles/profile_manager_observer.h"

class BraveOriginProfileObserver : public ProfileManagerObserver {
 public:
  BraveOriginProfileObserver();
  ~BraveOriginProfileObserver() override;

  void OnProfileAdded(Profile* profile) override;

 private:
  base::ScopedObservation<ProfileManager, ProfileManagerObserver> observation_{
      this};
};

#endif  // BRAVE_BROWSER_BRAVE_ORIGIN_BRAVE_ORIGIN_PROFILE_OBSERVER_H_
