/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import {PrefsMixin, PrefsMixinInterface} from '/shared/settings/prefs/prefs_mixin.js';
import {I18nMixin, I18nMixinInterface} from 'chrome://resources/cr_elements/i18n_mixin.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {BaseMixin, BaseMixinInterface} from '../base_mixin.js'
// import {loadTimeData} from '../i18n_setup.js'
import './memory_section.js'

import {getTemplate} from './customization_subpage.html.js'

const BraveLeoCustomizationSubpageBase =
    PrefsMixin(I18nMixin(BaseMixin(PolymerElement))) as {
      new (): PolymerElement & PrefsMixinInterface & I18nMixinInterface & BaseMixinInterface
    }

class BraveLeoCustomizationSubpage extends BraveLeoCustomizationSubpageBase {  
  static get is() {
    return 'leo-customization-subpage'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      name_: {
        type: String,
        computed: 'computePrefValue_(prefs.brave.ai_chat.user_customization_name)',
      },
      job_: {
        type: String,
        computed: 'computePrefValue_(prefs.brave.ai_chat.user_customization_job)',
      },
      tone_: {
        type: String,
        computed: 'computePrefValue_(prefs.brave.ai_chat.user_customization_tone)',
      },
      other_: {   
        type: String,
        computed: 'computePrefValue_(prefs.brave.ai_chat.user_customization_other)',
      },
      // Local properties for user input
      nameInput_: {
        type: String,
        value: '',
      },
      jobInput_: {
        type: String,
        value: '',
      },
      toneInput_: {
        type: String,
        value: '',
      },
      otherInput_: {
        type: String,
        value: '',
      },
    }
  }

  static get observers() {
    return [
      'prefsChanged_(prefs.brave.ai_chat.user_customization_name, prefs.brave.ai_chat.user_customization_job, prefs.brave.ai_chat.user_customization_tone, prefs.brave.ai_chat.user_customization_other)',
    ]
  }

  declare name_: string;
  declare job_: string;
  declare tone_: string;
  declare other_: string;
  
  // Local input properties
  declare nameInput_: string;
  declare jobInput_: string;
  declare toneInput_: string;
  declare otherInput_: string;
  
  override ready() {
    super.ready()
    // Initialize input values when prefs are ready
    this.initializeInputValues_()
  }

  private prefsChanged_() {
    // When prefs become available, initialize the input values
    if (this.prefs) {
      this.initializeInputValues_()
    }
  }

  private initializeInputValues_() {
    // Initialize input values from computed properties
    this.nameInput_ = this.name_
    this.jobInput_ = this.job_
    this.toneInput_ = this.tone_
    this.otherInput_ = this.other_
  }

  onSave() {
    // Store the current input values to preferences
    this.setPrefValue('brave.ai_chat.user_customization_name', this.nameInput_)
    this.setPrefValue('brave.ai_chat.user_customization_job', this.jobInput_)
    this.setPrefValue('brave.ai_chat.user_customization_tone', this.toneInput_)
    this.setPrefValue('brave.ai_chat.user_customization_other', this.otherInput_)
  }

  private computePrefValue_(pref: any): string {
    // Prefs might not be available yet
    if (!pref || !pref.value) {
      return ''
    }
    return pref.value
  }

  private onNameInputChanged_(event: any) {
    this.nameInput_ = event.value
  }

  private onJobInputChanged_(event: any) {
    this.jobInput_ = event.value
  }

  private onToneInputChanged_(event: any) {
    this.toneInput_ = event.value
  }

  private onOtherInputChanged_(event: any) {
    this.otherInput_ = event.value
  }
}

customElements.define(BraveLeoCustomizationSubpage.is, BraveLeoCustomizationSubpage)
