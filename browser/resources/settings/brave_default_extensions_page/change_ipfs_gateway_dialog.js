// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

/**
 * @fileoverview
 * 'change-ipfs-gateway-dialog' provides a dialog to configure the public
 * IPFS gateway address.
 */
import '../settings_shared_css.m.js';

import 'chrome://resources/cr_elements/cr_button/cr_button.m.js';
import 'chrome://resources/cr_elements/cr_dialog/cr_dialog.m.js';
import 'chrome://resources/cr_elements/cr_input/cr_input.m.js';

import {Polymer, html} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';
import {I18nBehavior} from 'chrome://resources/js/i18n_behavior.m.js';

Polymer({
  is: 'change-ipfs-gateway-dialog',

  _template: html`{__html_template__}`,

  behaviors: [
    I18nBehavior
  ],

  properties: {
    /**
     * Configured IPFS public gateway address.
     * @private
     */
    url_: String,

    /**
     * The error message to display when the input url is invalid.
     * @private
     */
    // errorMessage_: String,
  },

  /**
   * Validates that the url entered is valid.
   * @private
   */
  validate_ function() {
  },

  handleSubmit_: function() {
    // TODO: use browser proxy to set gateway
  },

});
