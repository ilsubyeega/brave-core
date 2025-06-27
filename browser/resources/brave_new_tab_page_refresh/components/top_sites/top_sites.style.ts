/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { color, font } from '@brave/leo/tokens/css/variables'
import { scoped } from '../../lib/scoped_css'

export const collapsedTileCount = 6
export const maxTileColumnCount = 12
export const maxTileRowCount = 2
export const maxPageSize = maxTileColumnCount * maxTileRowCount
export const maxPageCount = 4
export const tileWidth = 72
export const nonGridWidth = 64

export const style = scoped.css`
  & {
    --self-tile-width: ${tileWidth}px;
    --self-tile-height: 82px;
    --self-tile-icon-size: 56px;
    --self-transition-duration: 180ms;

    flex-grow: 1;
  }

  &.hidden {
    display: none;
  }

  .top-sites {
    display: flex;
    align-items: flex-start;
    justify-content: center;
    gap: 8px;
  }

  .top-site-tiles-mask {
    overflow-x: hidden;
    overflow-y: hidden;
    scrollbar-width: none;
    scroll-snap-type: x mandatory;
    max-width: calc(${collapsedTileCount} * var(--self-tile-width));
  }

  &.expanded .top-site-tiles-mask {
    overflow-x: scroll;
    max-width: var(--self-max-page-width, none);
  }

  .top-site-tiles {
    position: relative;
    display: grid;
    grid-auto-columns: var(--self-tile-width);
    grid-auto-rows: var(--self-tile-height);
    grid-row-gap: 16px;
  }

  .top-site-tile {
    width: var(--self-tile-width);
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 8px;
    text-decoration: none;

    opacity: 1;
    transition-property: opacity, display;
    transition-duration: var(--self-transition-duration);
    transition-delay: calc(var(--self-transition-duration) - 60ms);
    transition-behavior: allow-discrete;

    @starting-style {
      opacity: 0;
    }
  }

  .top-site-icon {
    --leo-icon-size: 32px;
    --leo-icon-color: ${color.white};

    position: relative;
    margin: 0 calc((var(--self-tile-width) - var(--self-tile-icon-size)) / 2);
    width: var(--self-tile-icon-size);
    height: var(--self-tile-icon-size);
    padding: 12px;
    border-radius: 16px;
    background: rgba(255, 255, 255, 0.25);
    backdrop-filter: blur(50px);
    display: flex;
    align-items: center;
    justify-content: center;

    /* Gradient border */
    &::before {
      content: "";
      position: absolute;
      inset: 0;
      border-radius: 16px;
      padding: 1px;
      background: linear-gradient(
        156.52deg,
        rgba(0, 0, 0, 0.05) 2.12%,
        rgba(0, 0, 0, 0) 39%,
        rgba(0, 0, 0, 0) 54.33%,
        rgba(0, 0, 0, 0.15) 93.02%);
      mask:
        linear-gradient(#fff 0 0) content-box,
        linear-gradient(#fff 0 0);
      mask-composite: exclude;
    }

    img {
      width: 100%;
      height: auto;
    }
  }

  .top-site-tile:hover .top-site-icon {
    background: rgba(255, 255, 255, .35);
  }

  .top-site-tile:focus-visible {
    outline: none;

    .top-site-icon {
      background: rgba(255, 255, 255, .35);
    }
  }

  .top-site-title {
    color: ${color.white};
    width: 100%;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    font: ${font.small.semibold};
    text-shadow: 0px 1px 4px rgba(0, 0, 0, 0.40);
    text-align: center;
  }

  .tile-drop-indicator {
    position: absolute;
    pointer-events: none;
    top: 0;
    left: 0;
    height: var(--self-tile-height);
    width: 4px;
    border-radius: 2px;
    background: ${color.divider.interactive};
    display: none;
    opacity: 0;

    transition: display 100ms allow-discrete, opacity 100ms;

    &.dragging {
      display: block;
      opacity: 1;

      @starting-style {
        opacity: 0;
      }
    }
  }

  .left-spacer {
    flex: 0 0 24px;
  }

  .menu-button {
    --leo-icon-color: rgba(255, 255, 255, .8);
    --leo-icon-size: 24px;

    anchor-name: --top-sites-menu-button;

    height: var(--self-tile-icon-size);
    display: flex;
    align-items: center;
    opacity: 0;
    border-radius: 12px;
    visibility: hidden;

    transition: opacity var(--self-transition-duration);

    &:focus-visible {
      opacity: 1;
      background: rgba(255, 255, 255, .35);
      outline: none;
    }
  }

  .menu-divider {
    border-top: solid 1px ${color.divider.subtle};
  }

  .top-sites-menu {
    position-anchor: --top-sites-menu-button;
    position-area: block-end span-inline-start;
    position-try-fallbacks: flip-block;
  }

  .top-site-context-menu-anchor {
    position: absolute;
    top: var(--self-context-menu-y, 0);
    left: var(--self-context-menu-x, 0);
    anchor-name: --top-site-context-menu-anchor;
  }

  .top-site-context-menu {
    position: absolute;
    position-anchor: --top-site-context-menu-anchor;
    position-area: block-end span-inline-end;
    position-try-fallbacks: flip-inline, flip-block;
  }

  .page-nav {
    --leo-navdots-active-color: #fff;
    --leo-navdots-active-color-hover: #fff;
    --leo-navdots-color: #fff;
    --leo-navdots-color-hover: #fff;

    margin-top: 16px;
    opacity: 1;
    transition: opacity 500ms;

    @starting-style {
      opacity: 0;
    }
  }

  &.collapsed .page-nav {
    opacity: 0;
    display: none;
  }

  &:hover .menu-button {
    opacity: 1;
    visibility: visible;
  }
`
