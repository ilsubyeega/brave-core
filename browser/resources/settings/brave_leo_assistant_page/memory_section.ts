// NOTE: Some imports may be project-specific and are expected to resolve in the Brave codebase.
import 'chrome://resources/cr_elements/cr_button/cr_button.js'
import 'chrome://resources/cr_elements/icons.html.js'

import { PrefsMixin } from '/shared/settings/prefs/prefs_mixin.js'
import { I18nMixin } from 'chrome://resources/cr_elements/i18n_mixin.js'
import { PolymerElement } from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'
import { BaseMixin } from '../base_mixin.js'
import { getTemplate } from './memory_section.html.js'
import { BraveLeoAssistantBrowserProxy, BraveLeoAssistantBrowserProxyImpl } from './brave_leo_assistant_browser_proxy.js'

const MemorySectionBase = PrefsMixin(I18nMixin(BaseMixin(PolymerElement)))

class MemorySection extends MemorySectionBase {
  static get is() {
    return 'memory-section'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      memoriesList_: {
        type: Array
      },
      isEditingMemoryIndex_: {
        type: Number,
        value: null
      },
      showMemoryDialog_: {
        type: Boolean,
        value: false
      },
      editingMemory_: {
        type: String,
        value: ''
      }
    }
  }

  browserProxy_: BraveLeoAssistantBrowserProxy =
    BraveLeoAssistantBrowserProxyImpl.getInstance()
  declare memoriesList_: string[]
  declare isEditingMemoryIndex_: number | null
  declare showMemoryDialog_: boolean
  declare editingMemory_: string

  override ready() {
    super.ready()
    this.fetchMemories_()
  }

  fetchMemories_() {
    // TODO: Replace with real API call
    this.memoriesList_ = []
  }

  saveMemory_(memoryItem: string, index: number | null) {
    // TODO: Call API to save memory
    console.log('saveMemory_', memoryItem, index)
    // Call API to save memory, pass -1 if adding a new memory
    // this.browserProxy_.saveMemory(memoryItem, index)
}

  onMemoryDialogSave_(e: { detail: { memoryItem: string } }) {
    this.showMemoryDialog_ = false
    this.isEditingMemoryIndex_ = null
    this.saveMemory_(e.detail.memoryItem, this.isEditingMemoryIndex_);
    this.fetchMemories_()
  }

  onMemoryDialogClose_() {
    this.isEditingMemoryIndex_ = null
    this.showMemoryDialog_ = false
  }

  handleDelete_(e: any) {
    const messageText = (this as any).i18n('braveLeoAssistantDeleteMemoryConfirmation')
    const shouldDelete = confirm(messageText)
    if (!shouldDelete) {
      return
    }
    // TODO: Call API to delete memory
    // this.browserProxy_.deleteMemory(e.model.index)
    console.log('handleDelete_', e)
    this.fetchMemories_()
  }

  handleEdit_(e: any) {
    this.isEditingMemoryIndex_ = e.model.index
    this.editingMemory_ = this.memoriesList_[e.model.index] || ''
    this.showMemoryDialog_ = true
  }

  handleAddNewMemory_() {
    this.isEditingMemoryIndex_ = null
    this.editingMemory_ = ''
    this.showMemoryDialog_ = true
  }

  onDialogInput_(e: any) {
    this.editingMemory_ = e.target.value
  }

  onDialogSave_() {
    this.saveMemory_(this.editingMemory_, this.isEditingMemoryIndex_)
    this.showMemoryDialog_ = false
    this.isEditingMemoryIndex_ = null
    this.editingMemory_ = ''
    this.fetchMemories_()
  }

  onDialogCancel_() {
    this.showMemoryDialog_ = false
    this.isEditingMemoryIndex_ = null
    this.editingMemory_ = ''
  }

  private getEditingMemory_(index: number, memoriesList: string[]) {
    return typeof index === 'number' && index >= 0 ? memoriesList[index] : ''
  }

  private hasMemories_(memoriesList: string[]) {
    return memoriesList.length > 0
  }
}

customElements.define(MemorySection.is, MemorySection) 