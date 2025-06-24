// NOTE: Some imports may be project-specific and are expected to resolve in the Brave codebase.
import 'chrome://resources/cr_elements/cr_button/cr_button.js'
import 'chrome://resources/cr_elements/cr_dialog/cr_dialog.js'
import 'chrome://resources/cr_elements/cr_input/cr_input.js'
import 'chrome://resources/cr_elements/icons.html.js'

import { PrefsMixin, PrefsMixinInterface } from '/shared/settings/prefs/prefs_mixin.js'
import { I18nMixin, I18nMixinInterface } from 'chrome://resources/cr_elements/i18n_mixin.js'
import { PolymerElement } from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'
import { BaseMixin, BaseMixinInterface } from '../base_mixin.js'
import { getTemplate } from './memory_section.html.js'
import { BraveLeoAssistantBrowserProxy, BraveLeoAssistantBrowserProxyImpl } from './brave_leo_assistant_browser_proxy.js'

const MemorySectionBase = PrefsMixin(I18nMixin(BaseMixin(PolymerElement))) as {
  new (): PolymerElement & PrefsMixinInterface & I18nMixinInterface & BaseMixinInterface
}

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
        type: Array,
        value: []
      },
      isEditingMemoryIndex_: {
        type: Number,
        value: -1
      },
      showMemoryDialog_: {
        type: Boolean,
        value: false
      },
      editingMemory_: {
        type: String,
        value: ''
      },
      isMemoryInputValid_: {
        type: Boolean,
        computed: 'computeMemoryInputValid_(editingMemory_)'
      },
      showDeleteDialog_: {
        type: Boolean,
        value: false
      },
      deleteMemoryIndex_: {
        type: Number,
        value: -1
      },
      showDeleteAllDialog_: {
        type: Boolean,
        value: false
      }
    }
  }

  static get observers() {
    return [
      'loadMemories_(prefs.brave.ai_chat.user_memories.value)',
    ]
  }

  browserProxy_: BraveLeoAssistantBrowserProxy =
    BraveLeoAssistantBrowserProxyImpl.getInstance()
  declare memoriesList_: string[]
  declare isEditingMemoryIndex_: number
  declare showMemoryDialog_: boolean
  declare editingMemory_: string
  declare isMemoryInputValid_: boolean
  declare showDeleteDialog_: boolean
  declare deleteMemoryIndex_: number
  declare showDeleteAllDialog_: boolean

  override ready() {
    super.ready()
    console.log('MemorySection ready, initial loadMemories_ call');
    this.loadMemories_();
  }

  loadMemories_() {
    console.log('loadMemories_ called');
    if (this.prefs) {
      console.log('prefs available:', this.prefs.brave.ai_chat.user_memories);
      this.memoriesList_ = [...this.prefs.brave.ai_chat.user_memories.value];
      console.log('memoriesList_ updated:', this.memoriesList_);
    } else {
      console.log('prefs not available');
    }
  }

  saveMemory_(memoryItem: string, index: number) {
    console.log('saveMemory_ called with:', memoryItem, index);
    const key = 'brave.ai_chat.user_memories';
    if (index === -1) {
      console.log('appending new memory');
      this.appendPrefListItem(key, memoryItem);
      // Manually reload memories immediately
      this.loadMemories_();
    } else {
      console.log('updating existing memory');
      this.set(`prefs.${key}.value.${index}`, memoryItem);
      // Manually reload memories immediately
      this.loadMemories_();
    }
  }

  onMemoryDialogSave_(e: { detail: { memoryItem: string } }) {
    this.showMemoryDialog_ = false
    this.isEditingMemoryIndex_ = -1
    this.saveMemory_(e.detail.memoryItem, this.isEditingMemoryIndex_);
  }

  onMemoryDialogClose_() {
    this.isEditingMemoryIndex_ = -1
    this.showMemoryDialog_ = false
  }

  handleDelete_(e: any) {
    this.deleteMemoryIndex_ = e.model.index;
    this.showDeleteDialog_ = true;
  }

  onDeleteDialogCancel_() {
    this.showDeleteDialog_ = false;
    this.deleteMemoryIndex_ = -1;
  }

  onDeleteDialogConfirm_() {
    if (this.deleteMemoryIndex_ !== -1) {
      const key = 'brave.ai_chat.user_memories';
      this.splice(`prefs.${key}.value`, this.deleteMemoryIndex_, 1);
      this.loadMemories_();
    }
    this.showDeleteDialog_ = false;
    this.deleteMemoryIndex_ = -1;
  }

  handleEdit_(e: any) {
    this.isEditingMemoryIndex_ = e.model.index
    this.editingMemory_ = this.memoriesList_[e.model.index] || ''
    this.showMemoryDialog_ = true
  }

  handleAddNewMemory_() {
    this.isEditingMemoryIndex_ = -1
    this.editingMemory_ = ''
    this.showMemoryDialog_ = true
  }

  onDialogInput_(e: any) {
    this.editingMemory_ = e.value
  }

  computeMemoryInputValid_(editingMemory: string): boolean {
    return editingMemory.trim().length > 0
  }

  computeHasMemories_(memoriesList: string[]): boolean {
    return memoriesList.length > 0
  }

  hasMemories_(memoriesList: string[]): boolean {
    return memoriesList.length > 0
  }

  onDialogSave_() {
    this.saveMemory_(this.editingMemory_, this.isEditingMemoryIndex_)
    this.showMemoryDialog_ = false
    this.isEditingMemoryIndex_ = -1
    this.editingMemory_ = ''
  }

  onDialogCancel_() {
    this.showMemoryDialog_ = false
    this.isEditingMemoryIndex_ = -1
    this.editingMemory_ = ''
  }

  private getEditingMemory_(index: number, memoriesList: string[]) {
    return typeof index === 'number' && index >= 0 ? memoriesList[index] : ''
  }

  private getDialogTitle_(isEditingMemoryIndex: number) {
    return isEditingMemoryIndex === -1 
      ? this.i18n('braveLeoAssistantCreateMemoryDialogTitle')
      : this.i18n('braveLeoAssistantEditMemoryDialogTitle')
  }

  private getInputPlaceholder_(editingMemory: string) {
    return editingMemory === '' 
      ? this.i18n('braveLeoAssistantMemoryInputPlaceholder')
      : ''
  }

  handleDeleteAllMemories_() {
    this.showDeleteAllDialog_ = true;
  }

  onDeleteAllDialogCancel_() {
    this.showDeleteAllDialog_ = false;
  }

  onDeleteAllDialogConfirm_() {
    const key = 'brave.ai_chat.user_memories';
    this.set(`prefs.${key}.value`, []);
    this.loadMemories_();
    this.showDeleteAllDialog_ = false;
  }
}

customElements.define(MemorySection.is, MemorySection) 