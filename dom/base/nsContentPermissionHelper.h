/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsContentPermissionHelper_h
#define nsContentPermissionHelper_h

#include "nsIContentPermissionPrompt.h"
#include "nsTArray.h"
#include "nsIMutableArray.h"
#include "PCOMContentPermissionRequestChild.h"

class nsPIDOMWindow;
class nsContentPermissionRequestProxy;

// Forward declare IPC::Principal here which is defined in
// PermissionMessageUtils.h. Include this file will transitively includes
// "windows.h" and it defines
//   #define CreateEvent CreateEventW
//   #define LoadImage LoadImageW
// That will mess up windows build.
namespace IPC {
class Principal;
}

namespace mozilla {
namespace dom {

class Element;
class PermissionRequest;
class ContentPermissionRequestParent;
class PContentPermissionRequestParent;

class ContentPermissionType : public nsIContentPermissionType
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICONTENTPERMISSIONTYPE

  ContentPermissionType(const nsACString& aType,
                        const nsACString& aAccess,
                        const nsTArray<nsString>& aOptions);
  virtual ~ContentPermissionType();

protected:
  nsCString mType;
  nsCString mAccess;
  nsTArray<nsString> mOptions;
};

uint32_t ConvertPermissionRequestToArray(nsTArray<PermissionRequest>& aSrcArray,
                                         nsIMutableArray* aDesArray);

nsresult CreatePermissionArray(const nsACString& aType,
                               const nsACString& aAccess,
                               const nsTArray<nsString>& aOptions,
                               nsIArray** aTypesArray);

PContentPermissionRequestParent*
CreateContentPermissionRequestParent(const nsTArray<PermissionRequest>& aRequests,
                                     Element* element,
                                     const IPC::Principal& principal);

} // namespace dom
} // namespace mozilla

class nsContentPermissionRequestProxy : public nsIContentPermissionRequest
{
 public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICONTENTPERMISSIONREQUEST

  nsContentPermissionRequestProxy();
  virtual ~nsContentPermissionRequestProxy();

  nsresult Init(const nsTArray<mozilla::dom::PermissionRequest>& requests,
                mozilla::dom::ContentPermissionRequestParent* parent);
  void OnParentDestroyed();

 private:
  // Non-owning pointer to the ContentPermissionRequestParent object which owns this proxy.
  mozilla::dom::ContentPermissionRequestParent* mParent;
  nsTArray<mozilla::dom::PermissionRequest> mPermissionRequests;
};

/**
+ * RemotePermissionRequest will send a prompt ipdl request to b2g process.
+ */
class RemotePermissionRequest : public nsIContentPermissionRequest
                              , public PCOMContentPermissionRequestChild
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICONTENTPERMISSIONREQUEST

  RemotePermissionRequest(nsIContentPermissionRequest* aRequest,
                          nsPIDOMWindow* aWindow);

  // It will be called when prompt dismissed.
  virtual bool Recv__delete__(const bool &aAllow,
                              const nsTArray<PermissionChoice>& aChoices) MOZ_OVERRIDE;
  virtual void IPDLRelease() MOZ_OVERRIDE { Release(); }

  static uint32_t ConvertArrayToPermissionRequest(
                                nsIArray* aSrcArray,
                                nsTArray<PermissionRequest>& aDesArray);
private:
  virtual ~RemotePermissionRequest() {}

  nsCOMPtr<nsIContentPermissionRequest> mRequest;
  nsCOMPtr<nsPIDOMWindow>               mWindow;
};

#endif // nsContentPermissionHelper_h
