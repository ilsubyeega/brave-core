/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { useBraveNews } from '../../../../components/brave_news/browser/resources/shared/Context'

import { useNewTabState } from '../context/new_tab_context'
import { useSearchState } from '../context/search_context'
import { useTopSitesState } from '../context/top_sites_context'
import { useRewardsState } from '../context/rewards_context'
import { useVpnState } from '../context/vpn_context'

export function useAppInitialized() {
  const newTabInitialized = useNewTabState((s) => s.initialized)
  const topSitesInitialized = useTopSitesState((s) => s.initialized)
  const searchInitialized = useSearchState((s) => s.initialized)
  const rewardsInitialized = useRewardsState((s) => s.initialized)
  const vpnInitialized = useVpnState((s) => s.initialized)
  const newsInitialized = useBraveNews().isShowOnNTPPrefEnabled !== undefined

  return (
    newTabInitialized && topSitesInitialized && searchInitialized &&
    rewardsInitialized && vpnInitialized && newsInitialized
  )
}
