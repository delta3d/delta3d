// base.cpp: Implementation of the Base class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/base.h"
#include <dtUtil/log.h>

#include <iostream>

using namespace dtCore;
using namespace dtUtil;

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
   Log &log = Log::GetInstance();

   log.LogMessage(Log::LOG_DEBUG, "base.cpp", "Destroying '%s'", GetName().c_str());
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

/** Subscribe this instance to the supplied sender.  Any message that sender
  * sends, will be received in the OnMessage() method.
  * @param sender The sender that this instance should listen to
  */
void Base::AddSender( Base *sender)
{
   //connect the sender's signal to our slot
   sender->_sendMessage.connect_slot( this, &Base::OnMessage );
}

///Stop receiving messages from the supplied sender instance
void Base::RemoveSender( Base *sender )
{
   sender->_sendMessage.disconnect( this );
}

/** Send a message to any instance thats subscribed to us.
  * @param message Optional text message (def = "")
  * @param data Optional void pointer to any user data (def = NULL)
  */
void Base::SendMessage(const std::string& message, void *data)
{
   //make a new MessageData, load it up, and pass it to our signal
   MessageData dataToSend;
   dataToSend.message = message;
   dataToSend.sender = this;
   dataToSend.userData = data;
   _sendMessage( &dataToSend );
}
