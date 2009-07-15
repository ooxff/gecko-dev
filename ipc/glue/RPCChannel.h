/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: sw=4 ts=4 et :
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Plugin App.
 *
 * The Initial Developer of the Original Code is
 *   Chris Jones <jones.chris.g@gmail.com>
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef ipc_glue_RPCChannel_h
#define ipc_glue_RPCChannel_h 1

// FIXME/cjones probably shouldn't depend on this
#include <stack>

#include "mozilla/ipc/SyncChannel.h"

namespace mozilla {
namespace ipc {
//-----------------------------------------------------------------------------

class RPCChannel : public SyncChannel
{
public:
    class RPCListener : public SyncChannel::Listener
    {
    public:
        virtual ~RPCListener() { }
        virtual Result OnMessageReceived(const Message& aMessage) = 0;
        virtual Result OnMessageReceived(const Message& aMessage,
                                         Message*& aReply) = 0;
        virtual Result OnCallReceived(const Message& aMessage,
                                      Message*& aReply) = 0;
    };
    typedef RPCListener Listener;

    RPCChannel(Listener* aListener) :
        SyncChannel(aListener)
    {
    }

    virtual ~RPCChannel()
    {
        // FIXME/cjones: impl
    }

    // Make an RPC to the other side of the channel
    bool Call(Message* msg, Message* reply);

    // Override the SyncChannel handler so we can dispatch RPC messages
    virtual void OnMessageReceived(const Message& msg);

private:
    // Executed on worker thread
    virtual bool WaitingForReply() {
        mMutex.AssertCurrentThreadOwns();
        return mPending.size() > 0 || SyncChannel::WaitingForReply();
    }

    void OnDispatchMessage(const Message& msg);

    std::stack<Message> mPending;
};


} // namespace ipc
} // namespace mozilla
#endif  // ifndef ipc_glue_RPCChannel_h
