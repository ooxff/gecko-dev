/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_mobilemessage_MobileMessageManager_h
#define mozilla_dom_mobilemessage_MobileMessageManager_h

#include "mozilla/DOMEventTargetHelper.h"
#include "nsIDOMMobileMessageManager.h"
#include "nsIObserver.h"

class nsIDOMMozSmsMessage;
class nsIDOMMozMmsMessage;

namespace mozilla {
namespace dom {

class MobileMessageManager : public DOMEventTargetHelper
                           , public nsIDOMMozMobileMessageManager
                           , public nsIObserver
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIOBSERVER
  NS_DECL_NSIDOMMOZMOBILEMESSAGEMANAGER

  NS_REALLY_FORWARD_NSIDOMEVENTTARGET(DOMEventTargetHelper)

  void Init(nsPIDOMWindow *aWindow);
  void Shutdown();

private:
  /**
   * Internal Send() method used to send one message.
   */
  nsresult Send(JSContext* aCx, JS::Handle<JSObject*> aGlobal,
                uint32_t aServiceId,
                JS::Handle<JSString*> aNumber,
                const nsAString& aMessage,
                JS::MutableHandle<JS::Value> aRequest);

  nsresult DispatchTrustedSmsEventToSelf(const char* aTopic,
                                         const nsAString& aEventName,
                                         nsISupports* aMsg);

  nsresult DispatchTrustedDeletedEventToSelf(nsISupports* aDeletedInfo);

  /**
   * Helper to get message ID from SMS/MMS Message object
   */
  nsresult GetMessageId(JSContext* aCx, const JS::Value& aMessage,
                        int32_t* aId);
};

} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_mobilemessage_MobileMessageManager_h
