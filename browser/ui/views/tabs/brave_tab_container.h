/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_

#include <memory>
#include <optional>
#include <vector>

#include "brave/browser/ui/tabs/split_view_browser_data.h"
#include "brave/browser/ui/tabs/split_view_browser_data_observer.h"
#include "chrome/browser/ui/tabs/tab_style.h"
#include "chrome/browser/ui/views/tabs/dragging/tab_drag_context.h"
#include "chrome/browser/ui/views/tabs/tab_container_impl.h"
#include "ui/gfx/canvas.h"

class BraveTabContainer : public TabContainerImpl,
                          public SplitViewBrowserDataObserver {
  METADATA_HEADER(BraveTabContainer, TabContainerImpl)
 public:
  BraveTabContainer(TabContainerController& controller,
                    TabHoverCardController* hover_card_controller,
                    TabDragContextBase* drag_context,
                    TabSlotController& tab_slot_controller,
                    views::View* scroll_contents_view);
  ~BraveTabContainer() override;

  // Calling this will freeze this view's layout. When the returned closure
  // runs, layout will be unlocked and run immediately.
  // This is to avoid accessing invalid index during reconstruction
  // of TabContainer. In addition, we can avoid redundant layout as a side
  // effect.
  base::OnceClosure LockLayout();

  // TabContainerImpl:
  void AddedToWidget() override;
  gfx::Size CalculatePreferredSize(
      const views::SizeBounds& available_size) const override;
  void UpdateClosingModeOnRemovedTab(int model_index, bool was_active) override;
  gfx::Rect GetTargetBoundsForClosingTab(Tab* tab,
                                         int former_model_index) const override;
  void EnterTabClosingMode(std::optional<int> override_width,
                           CloseTabSource source) override;
  bool ShouldTabBeVisible(const Tab* tab) const override;
  void StartInsertTabAnimation(int model_index) override;
  void RemoveTab(int index, bool was_active) override;
  void OnTabCloseAnimationCompleted(Tab* tab) override;
  void CompleteAnimationAndLayout() override;
  void PaintChildren(const views::PaintInfo& paint_info) override;
  void SetTabSlotVisibility() override;
  void InvalidateIdealBounds() override;
  void Layout(PassKey) override;
  void OnSplitCreated(const std::vector<int>& indices) override;
  void OnSplitRemoved(const std::vector<int>& indices) override;
  void OnSplitContentsChanged(const std::vector<int>& indices) override;

  // BrowserRootView::DropTarget
  std::optional<BrowserRootView::DropIndex> GetDropIndex(
      const ui::DropTargetEvent& event) override;
  void HandleDragUpdate(
      const std::optional<BrowserRootView::DropIndex>& index) override;
  void HandleDragExited() override;

  // SplitViewBrowserDataObserver:
  void OnTileTabs(const TabTile& tile) override;
  void OnDidBreakTile(const TabTile& tile) override;
  void OnSwapTabsInTile(const TabTile& tile) override;
  void OnWillDeleteBrowserData() override;

 private:
  class DropArrow {
   public:
    enum class Position { Vertical, Horizontal };

    DropArrow(const BrowserRootView::DropIndex& index,
              Position position,
              bool beneath,
              views::Widget* context);
    DropArrow(const DropArrow&) = delete;
    DropArrow& operator=(const DropArrow&) = delete;
    virtual ~DropArrow();

    void set_index(const BrowserRootView::DropIndex& index) { index_ = index; }
    BrowserRootView::DropIndex index() const { return index_; }

    void SetBeneath(bool beneath);
    bool beneath() const { return beneath_; }

    void SetWindowBounds(const gfx::Rect& bounds);

   private:
    // Index of the tab to drop on.
    BrowserRootView::DropIndex index_;

    Position position_ = Position::Vertical;

    bool beneath_ = false;

    // Renders the drop indicator.
    std::unique_ptr<views::Widget> arrow_window_;
    raw_ptr<views::ImageView, DanglingUntriaged> arrow_view_ = nullptr;
  };

  void UpdateLayoutOrientation();

  void PaintBoundingBoxForTiles(gfx::Canvas& canvas,
                                const SplitViewBrowserData* split_view_data);
  void PaintBoundingBoxForTile(gfx::Canvas& canvas, const TabTile& tile);
  void PaintBoundingBoxForSplitTabs(gfx::Canvas& canvas);
  void PaintBoundingBoxForSplitTab(gfx::Canvas& canvas,
                                   const std::vector<int>& indices);

  static gfx::ImageSkia* GetDropArrowImage(
      BraveTabContainer::DropArrow::Position pos,
      bool beneath);

  void OnUnlockLayout();

  void SetDropArrow(const std::optional<BrowserRootView::DropIndex>& index);
  gfx::Rect GetDropBounds(int drop_index,
                          bool drop_before,
                          bool drop_in_group,
                          bool* is_beneath);

  bool IsPinnedTabContainer() const;
  void UpdateTabsBorderInTile(const TabTile& tile);
  void UpdateTabsBorderInSplitTab(const std::vector<int>& indices);

  base::flat_set<Tab*> closing_tabs_;

  raw_ptr<TabDragContextBase> drag_context_;

  // A pointer storing the global tab style to be used.
  const raw_ptr<const TabStyle> tab_style_;

  const raw_ref<TabContainerController, DanglingUntriaged> controller_;

  std::unique_ptr<DropArrow> drop_arrow_;

  BooleanPrefMember show_vertical_tabs_;
  BooleanPrefMember vertical_tabs_floating_mode_enabled_;
  BooleanPrefMember vertical_tabs_collapsed_;

  bool layout_locked_ = false;

  // Size we last laid out at.
  gfx::Size last_layout_size_;

  base::ScopedObservation<SplitViewBrowserData, SplitViewBrowserDataObserver>
      split_view_data_observation_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_
