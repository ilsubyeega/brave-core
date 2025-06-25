// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/ios/browser/api/brave_shields/brave_shields_utils_ios.h"

#include <Foundation/Foundation.h>

#include "brave/components/brave_shields/core/browser/brave_shields_utils.h"
#include "ios/chrome/browser/content_settings/model/host_content_settings_map_factory.h"
#include "ios/chrome/browser/shared/model/application_context/application_context.h"
#include "ios/chrome/browser/shared/model/profile/profile_ios.h"
#include "ios/web/public/thread/web_thread.h"
#include "net/base/apple/url_conversions.h"
#include "url/gurl.h"

@interface BraveShieldsUtilsIOS () {
}

@end

@implementation BraveShieldsUtilsIOS {
  raw_ptr<ProfileIOS> _profile;  // NOT OWNED
}

- (instancetype)initWithBrowserState:(ProfileIOS*)profile {
  if ((self = [super init])) {
    DCHECK_CURRENTLY_ON(web::WebThread::UI);
    _profile = profile;
  }
  return self;
}

- (bool)getBraveShieldsEnabledFor:(NSURL*)url {
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  return brave_shields::GetBraveShieldsEnabled(map, gurl);
}

- (void)setBraveShieldsEnabled:(bool)isEnabled forURL:(NSURL*)url {
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  auto* local_state = GetApplicationContext()->GetLocalState();
  brave_shields::SetBraveShieldsEnabled(map, isEnabled, gurl, local_state);
}

- (BraveShieldsAdBlockMode)getAdBlockModeForURL:(NSURL*)url {
  GURL gurl = net::GURLWithNSURL(url);
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);

  brave_shields::ControlType control_type_ad =
      brave_shields::GetAdControlType(map, gurl);

  brave_shields::ControlType control_type_cosmetic =
      brave_shields::GetCosmeticFilteringControlType(map, gurl);

  if (control_type_ad == brave_shields::ControlType::ALLOW) {
    return BraveShieldsAdBlockModeAllow;
  }

  if (control_type_cosmetic == brave_shields::ControlType::BLOCK) {
    return BraveShieldsAdBlockModeAggressive;
  } else {
    return BraveShieldsAdBlockModeStandard;
  }
}

- (void)setAdBlockMode:(BraveShieldsAdBlockMode)adBlockMode forURL:(NSURL*)url {
  brave_shields::ControlType control_type_cosmetic;
  switch (adBlockMode) {
    case BraveShieldsAdBlockModeAggressive:
      control_type_cosmetic = brave_shields::ControlType::BLOCK;  // aggressive
      break;
    case BraveShieldsAdBlockModeStandard:
      control_type_cosmetic =
          brave_shields::ControlType::BLOCK_THIRD_PARTY;  // standard
      break;
    case BraveShieldsAdBlockModeAllow:
      control_type_cosmetic = brave_shields::ControlType::ALLOW;  // allow
      break;
  }
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  auto* local_state = GetApplicationContext()->GetLocalState();
  auto* profile_prefs = _profile->GetPrefs();

  brave_shields::SetCosmeticFilteringControlType(
      map, control_type_cosmetic, gurl, local_state, profile_prefs);
}

- (bool)getBlockScriptsEnabledForURL:(NSURL*)url {
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  brave_shields::ControlType control_type =
      brave_shields::GetNoScriptControlType(map, gurl);

  if (control_type == brave_shields::ControlType::ALLOW) {
    return false;
  }

  return true;
}

- (void)setBlockScriptsEnabled:(bool)isEnabled forURL:(NSURL*)url {
  brave_shields::ControlType control_type;

  if (!isEnabled) {
    control_type = brave_shields::ControlType::ALLOW;
  } else {
    control_type = brave_shields::ControlType::BLOCK;
  }
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  auto* local_state = GetApplicationContext()->GetLocalState();

  brave_shields::SetNoScriptControlType(map, control_type, gurl, local_state);
}

- (bool)getBlockFingerprintingEnabledForURL:(NSURL*)url {
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  brave_shields::ControlType control_type =
      brave_shields::GetFingerprintingControlType(map, gurl);

  if (control_type == brave_shields::ControlType::ALLOW) {
    return false;
  } else if (control_type == brave_shields::ControlType::BLOCK) {
    return true;
  } else {  // brave_shields::ControlType::DEFAULT
    return true;
  }
}

- (void)setBlockFingerprintingEnabled:(bool)isEnabled forURL:(NSURL*)url {
  brave_shields::ControlType control_type;

  if (isEnabled) {
    control_type = brave_shields::ControlType::DEFAULT;  // STANDARD_MODE
  } else {
    // ControlType::ALLOW to allow fingerprinting
    control_type = brave_shields::ControlType::ALLOW;
  }

  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  auto* local_state = GetApplicationContext()->GetLocalState();
  auto* profile_prefs = _profile->GetPrefs();
  brave_shields::SetFingerprintingControlType(map, control_type, gurl,
                                              local_state, profile_prefs);
}

- (BraveShieldsAutoShredMode)getAutoShredModeForURL:(NSURL*)url
    NS_SWIFT_NAME(autoShredMode(for:)) {
  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  brave_shields::AutoShredType auto_shred_type =
      brave_shields::GetAutoShredType(map, gurl);

  if (auto_shred_type == brave_shields::AutoShredType::APP_EXIT) {
    return BraveShieldsAutoShredModeAppExit;
  } else if (auto_shred_type == brave_shields::AutoShredType::TAB_CLOSE) {
    return BraveShieldsAutoShredModeTabClose;
  } else {
    return BraveShieldsAutoShredModeNever;
  }
}

- (void)setAutoShredMode:(BraveShieldsAutoShredMode)mode forURL:(NSURL*)url {
  brave_shields::AutoShredType auto_shred_type;
  switch (mode) {
    case BraveShieldsAutoShredModeAppExit:
      auto_shred_type = brave_shields::AutoShredType::APP_EXIT;
      break;
    case BraveShieldsAutoShredModeTabClose:
      auto_shred_type = brave_shields::AutoShredType::TAB_CLOSE;
      break;
    case BraveShieldsAutoShredModeNever:
      auto_shred_type = brave_shields::AutoShredType::NEVER;
      break;
  }

  HostContentSettingsMap* map =
      ios::HostContentSettingsMapFactory::GetForProfile(_profile);
  GURL gurl = net::GURLWithNSURL(url);
  auto* local_state = GetApplicationContext()->GetLocalState();

  brave_shields::SetAutoShredType(map, auto_shred_type, gurl, local_state);
}

@end
