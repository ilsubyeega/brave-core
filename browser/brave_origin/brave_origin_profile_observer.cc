/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_origin/brave_origin_profile_observer.h"

#include "brave/components/ai_chat/core/common/pref_names.h"
#include "brave/components/brave_news/common/pref_names.h"
#include "brave/components/brave_origin/brave_origin_state.h"
#include "brave/components/brave_rewards/core/pref_names.h"
#include "brave/components/brave_vpn/common/pref_names.h"
#include "brave/components/brave_wallet/common/pref_names.h"
#include "brave/components/sidebar/browser/pref_names.h"
#include "brave/components/sidebar/browser/sidebar_item.h"
#include "brave/components/tor/pref_names.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "components/prefs/pref_service.h"

BraveOriginProfileObserver::BraveOriginProfileObserver() {
  observation_.Observe(g_browser_process->profile_manager());
}

BraveOriginProfileObserver::~BraveOriginProfileObserver() = default;

void BraveOriginProfileObserver::OnProfileAdded(Profile* profile) {
  if (!profile || profile->IsIncognitoProfile()) {
    return;
  }

  if (BraveOriginState::GetInstance()->IsBraveOriginUser()) {
    PrefService* prefs = profile->GetPrefs();
    // prefs->SetDefaultPrefValue(brave_rewards::prefs::kShowLocationBarButton,
    // base::Value(false));
    // prefs->SetDefaultPrefValue(brave_vpn::prefs::kBraveVPNShowButton,
    // base::Value(false)); prefs->SetDefaultPrefValue(kShowWalletIconOnToolbar,
    // base::Value(false));
    // prefs->SetDefaultPrefValue(ai_chat::prefs::kBraveAIChatShowToolbarButton,
    // base::Value(false));
    // prefs->SetDefaultPrefValue(ai_chat::prefs::kBraveAIChatContextMenuEnabled,
    // base::Value(false));
    // prefs->SetDefaultPrefValue(ai_chat::prefs::kBraveChatAutocompleteProviderEnabled,
    // base::Value(false));

    // Disable BraveVPNDisabled admin policy for Brave Origin profiles
    // Managed preferences
    prefs->SetDefaultPrefValue(brave_vpn::prefs::kManagedBraveVPNDisabled,
                               base::Value(true));
    prefs->SetDefaultPrefValue(ai_chat::prefs::kEnabledByPolicy,
                               base::Value(false));
    prefs->SetDefaultPrefValue(brave_wallet::prefs::kDisabledByPolicy,
                               base::Value(true));
    prefs->SetDefaultPrefValue(brave_rewards::prefs::kDisabledByPolicy,
                               base::Value(true));
    // TODO search ads!
    // TODO Email alias!
    // TODO Playlist
    // Tor Windows

    prefs->SetDefaultPrefValue(brave_news::prefs::kNewTabPageShowToday,
                               base::Value(false));
    prefs->SetDefaultPrefValue(brave_news::prefs::kShouldShowToolbarButton,
                               base::Value(false));

    // Not working
    // base::Value::List hidden_items;
    // hidden_items.Append(static_cast<int>(sidebar::SidebarItem::BuiltInItemType::kBraveTalk));
    // hidden_items.Append(static_cast<int>(sidebar::SidebarItem::BuiltInItemType::kWallet));
    // hidden_items.Append(static_cast<int>(sidebar::SidebarItem::BuiltInItemType::kChatUI));
    // prefs->SetDefaultPrefValue(sidebar::kSidebarHiddenBuiltInItems,
    // base::Value(std::move(hidden_items)));

    // TODO: Better way to set local state default prefs? There is no
    // SetDefaultPrefValue
    g_browser_process->local_state()->SetBoolean(tor::prefs::kTorDisabled,
                                                 true);
    // prefs->SetDefaultPrefValue(tor::prefs::kTorDisabled, base::Value(true));

    // Cache is not reapplied until you do this
#if 0
    if (!prefs->HasPrefPath(brave_rewards::prefs::kShowLocationBarButton)) {
      prefs->ClearPref(brave_rewards::prefs::kShowLocationBarButton);
    }
#endif
  }
}
