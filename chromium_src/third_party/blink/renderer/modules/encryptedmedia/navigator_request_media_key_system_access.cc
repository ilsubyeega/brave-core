/* Copyright (c) 2018 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

namespace blink {
class LocalFrame;

namespace {
class MediaKeySystemAccessInitializer;

void MaybeOnWidevineRequest(MediaKeySystemAccessInitializer* initializer,
                            LocalFrame* frame);
}  // namespace
}  // namespace blink

// Notify Brave about the widevine availability request.
#define BRAVE_NAVIGATOR_REQUEST_MEDIA_KEY_SYSTEM_ACCESS \
  MaybeOnWidevineRequest(initializer, window->GetFrame());

#include "src/third_party/blink/renderer/modules/encryptedmedia/navigator_request_media_key_system_access.cc"

#include "base/check.h"
#undef BRAVE_NAVIGATOR_REQUEST_MEDIA_KEY_SYSTEM_ACCESS

#include "brave/components/brave_drm/brave_drm.mojom-blink.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"

namespace blink {
namespace {

// Notifies Brave about the widevine availability request.
void MaybeOnWidevineRequest(MediaKeySystemAccessInitializer* initializer,
                            LocalFrame* frame) {
  if (initializer->KeySystem() == "com.widevine.alpha") {
    if (frame->Client()->GetRemoteNavigationAssociatedInterfaces()) {
      mojo::AssociatedRemote<brave_drm::mojom::blink::BraveDRM>
          brave_drm_binding;
      frame->Client()->GetRemoteNavigationAssociatedInterfaces()->GetInterface(
          &brave_drm_binding);
      DCHECK(brave_drm_binding.is_bound());
      brave_drm_binding->OnWidevineKeySystemAccessRequest();
    }
  }
}

}  // namespace
}  // namespace blink
