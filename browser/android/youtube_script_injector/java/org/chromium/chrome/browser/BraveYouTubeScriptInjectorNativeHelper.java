/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser;

import org.jni_zero.JNINamespace;
import org.jni_zero.NativeMethods;

import org.chromium.build.annotations.NullMarked;
import org.chromium.content_public.browser.WebContents;

/**
 * Helper to interact with native methods. Check brave_youtube_script_injector_native_helper.{h|cc}.
 */
@JNINamespace("youtube_script_injector")
@NullMarked
public class BraveYouTubeScriptInjectorNativeHelper {
    public static void setFullscreen(WebContents webContents) {
        BraveYouTubeScriptInjectorNativeHelperJni.get().setFullscreen(webContents);
    }

    public static boolean hasFullscreenBeenRequested(WebContents webContents) {
        return BraveYouTubeScriptInjectorNativeHelperJni.get()
                .hasFullscreenBeenRequested(webContents);
    }

    public static boolean isPictureInPictureAvailable(WebContents webContents) {
        return BraveYouTubeScriptInjectorNativeHelperJni.get()
                .isPictureInPictureAvailable(webContents);
    }

    /**
     * @noinspection unused
     */
    @NativeMethods
    interface Natives {
        void setFullscreen(WebContents webContents);

        boolean hasFullscreenBeenRequested(WebContents webContents);

        boolean isPictureInPictureAvailable(WebContents webContents);
    }
}
