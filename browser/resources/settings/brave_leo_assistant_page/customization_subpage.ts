/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import {PrefsMixin} from '/shared/settings/prefs/prefs_mixin.js';
import {I18nMixin} from 'chrome://resources/cr_elements/i18n_mixin.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {BaseMixin} from '../base_mixin.js'
// import {loadTimeData} from '../i18n_setup.js'

import {getTemplate} from './customization_subpage.html.js'

const BraveLeoCustomizationSubpageBase =
    PrefsMixin(I18nMixin(BaseMixin(PolymerElement)))

class BraveLeoCustomizationSubpage extends BraveLeoCustomizationSubpageBase {
  static get is() {
    return 'leo-customization-subpage'
  }

  static get template() {
    return getTemplate()
  }
}

customElements.define(BraveLeoCustomizationSubpage.is, BraveLeoCustomizationSubpage)
