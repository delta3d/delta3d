/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/base.h>
#include <dtUtil/log.h>

using namespace dtUtil;

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(Base)

/**
 * Constructor.
 *
 * @param name the instance name
 */
Base::Base(const std::string& name) : mName(name)
{
   RegisterInstance(this);
}

/**
 * Destructor.
 */
Base::~Base()
{
   Log& log = Log::GetInstance();

   log.LogMessage(Log::LOG_DEBUG, __FUNCTION__, "Destroying '%s'", GetName().c_str());
   DeregisterInstance(this);
}

/**
 * Sets the name of this instance.
 *
 * @param name the new name
 */
void Base::SetName(const std::string& name)
{
   mName = name;
}

/**
 * Returns the name of this instance.
 *
 * @return the current name
 */
const std::string& Base::GetName() const
{
   return mName;
}

/**
 * Subscribe this instance to the supplied sender.  Any message that sender
 * sends, will be received in the OnMessage() method.
 * @param sender The sender that this instance should listen to
 */
void Base::AddSender(Base* sender)
{
   //connect the sender's signal to our slot
   sender->mSendMessage.connect_slot(this, &Base::OnMessage);
}

/**
 * Stop receiving messages from the supplied sender instance
 */
void Base::RemoveSender(Base* sender)
{
   sender->mSendMessage.disconnect(this);
}

/**
 * Send a message to any instance thats subscribed to us.
 * @param message Optional text message (def = "")
 * @param data Optional void pointer to any user data (def = 0)
 */
void Base::SendMessage(const std::string& message, void* data)
{
   //make a new MessageData, load it up, and pass it to our signal
   MessageData dataToSend;
   dataToSend.message = message;
   dataToSend.sender = this;
   dataToSend.userData = data;
   mSendMessage(&dataToSend);
}

}
