/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import Icon from '@brave/leo/react/icon'
import NavDots from '@brave/leo/react/navdots'

import { TopSite } from '../../state/top_sites_state'
import { useTopSitesState, useTopSitesActions } from '../../context/top_sites_context'
import { getString } from '../../lib/strings'
import { TopSitesTile, DropLocation } from './top_site_tile'

import {
  tileWidth,
  collapsedTileCount,
  nonGridWidth,
  maxPageCount,
  maxTileRowCount,
  maxPageSize } from './top_sites.style'

interface Props {
  expanded: boolean
  canAddSite: boolean
  canReorderSites: boolean
  onAddTopSite: () => void
  onTopSiteContextMenu: (topSite: TopSite, event: React.MouseEvent) => void
}

export function TopSitesGrid(props: Props) {
  const actions = useTopSitesActions()
  const topSites = useTopSitesState((s) => s.topSites)

  const [scrollPage, setScrollPage] = React.useState(0)
  const [columnsPerPage, setColumnsPerPage] = React.useState(0)
  const scrollRef = React.useRef<HTMLDivElement>(null)

  React.useEffect(() => {
    const elem = scrollRef.current
    if (!elem) {
      return
    }
    const onResize = () => {
      const availableWidth =
        elem.closest('.top-sites')!.clientWidth - nonGridWidth
      const columns = Math.floor(availableWidth / tileWidth)
      setColumnsPerPage(columns)
      elem.style.setProperty(
        '--self-max-page-width', `${columns * tileWidth}px`)
    }
    onResize()
    const observer = new ResizeObserver(onResize)
    observer.observe(elem)
    return () => observer.disconnect()
  }, [])

  const tileCount = topSites.length + (props.canAddSite ? 1 : 0)
  const pageSize = Math.min(maxPageSize, columnsPerPage * maxTileRowCount)
  const pageWidth = columnsPerPage * tileWidth
  const pageCount = Math.ceil(pageSize === 0 ? 0 : tileCount / pageSize)
  const maxTileCount =
    props.expanded ? pageSize * maxPageCount : collapsedTileCount
  const layoutHelper = createGridLayoutHelper(columnsPerPage, pageSize)

  function onTileDrop(position: number) {
    return (url: string, location: DropLocation) => {
      const current = topSites.findIndex((item) => item.url === url)
      if (current < 0) {
        return
      }
      let index = position;
      if (location === 'after' && index < current) {
        index += 1
      }
      actions.setTopSitePosition(url, index)
    }
  }

  function onScroll() {
    const elem = scrollRef.current
    if (!elem) {
      return
    }
    setScrollPage(Math.round(elem.scrollLeft / pageWidth))
  }

  function scrollToPage(page: number) {
    scrollRef.current?.scrollTo({
      left: page * pageWidth,
      behavior: 'smooth'
    })
  }

  return (
    <div>
      <div
        ref={scrollRef}
        className='top-site-tiles-mask'
        onScroll={onScroll}
      >
        <div className='top-site-tiles'>
          {
            topSites.map((topSite, i) => {
              if (i > maxTileCount || pageSize === 0) {
                return null
              }
              return (
                <TopSitesTile
                  key={i}
                  topSite={topSite}
                  style={layoutHelper.next()}
                  canDrag={props.canReorderSites}
                  onDrop={onTileDrop(i)}
                  onRightClick={(event) => {
                    return props.onTopSiteContextMenu(topSite, event)
                  }}
                />
              )
            })
          }
          {
            props.canAddSite &&
              <button
                className='top-site-tile'
                style={layoutHelper.next()}
                onClick={props.onAddTopSite}
              >
                <span className='top-site-icon'>
                  <Icon name='plus-add' />
                </span>
                <span className='top-site-title'>
                  {getString('addTopSiteLabel')}
                </span>
              </button>
          }
          {
            layoutHelper.remainingColumns().map((i) =>
              <div key={-i} style={layoutHelper.next()} />
            )
          }
          <div className='tile-drop-indicator' />
        </div>
      </div>
      {
        pageCount > 1 &&
          <div className='page-nav'>
            <NavDots
              dotCount={pageCount}
              activeDot={scrollPage}
              onChange={(event) => scrollToPage(event.activeDot)}
            />
          </div>
      }
    </div>
  )
}

function createGridLayoutHelper(columnsPerPage: number, pageSize: number) {
  let currentColumn = 0
  let currentColumnMax = columnsPerPage
  let currentRow = 1
  let currentPage = 1

  function resetCurrentColumn() {
    currentColumn = (currentPage - 1) * columnsPerPage + 1
    currentColumnMax = currentColumn + columnsPerPage - 1
  }

  function next(): React.CSSProperties {
    if (pageSize === 0) {
      return {}
    }
    currentColumn += 1
    if (currentColumn > currentColumnMax) {
      resetCurrentColumn()
      currentRow += 1
      if (currentRow > maxTileRowCount) {
        currentPage += 1
        resetCurrentColumn()
        currentRow = 1
      }
    }
    return {
      gridColumn: currentColumn,
      gridRow: currentRow,
      scrollSnapAlign: currentColumn % columnsPerPage === 1 ? 'start' : 'none'
    }
  }

  function remainingColumns() {
    const remaining = currentColumnMax - currentColumn
    if (remaining <= 0) {
      return []
    }
    return [...Array(remaining).keys()]
  }

  return { next, remainingColumns }
}
