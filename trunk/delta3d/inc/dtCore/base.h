/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_BASE
#define DELTA_BASE

// base.h: The root of the inheritance hierarchy.
//
//////////////////////////////////////////////////////////////////////


#include <string>

#include "dtCore/id.h"
#include "dtCore/export.h"
#include "dtCore/macros.h"

#include <osg/Referenced>

#include "sigslot.h"

namespace dtCore
{
   ///Base class to support naming and message passing

   /** The Base class handles things that are required by most of the dtCore 
     * classes such as naming, RTTI, and message passing.
     * To name an instance, call SetName() or pass it to the constructor.
     *
     * Inter-class message passing is handled by "subscribing" the 
     * instance to a sender using AddSender().  Anytime the sender
     * calls SendMessage(), the receiver class' OnMessage() will get triggered.
     * The MessageData that gets passed to OnMessage() contains a pointer to the
     * sender, and optionally, a text message and pointer to user data.
     *
     * This class is also reference counted using the osg::Referenced class.  To
     * safely keep the reference count up-to-date, pointers to Base classes should
     * be stored in a RefPtr template.  For example:
     * \code 
     * RefPtr<Base> mPointerToMyBase;
     * \endcode
     */
   class DT_EXPORT Base : public sigslot::has_slots<>, public osg::Referenced
   {
      DECLARE_MANAGEMENT_LAYER(Base)


      public:

         ///Data that gets passed through SendMessage
         struct DT_EXPORT MessageData{
            std::string message;   ///<Textual message
            Base *sender;     ///<Pointer to the sender
            void *userData;   ///<Void pointer to user data
         };
         
      private:

         /**
          * The name of this instance.
          */
         std::string mName;
         
         ///The actual signal that gets triggered from SendMessage()
         sigslot::signal1<MessageData *> _sendMessage;
         
         Id mId;

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Base(std::string name = "base");

         /**
          * Destructor.
          */
         virtual ~Base();

         /**
          * Sets the name of this instance.
          *
          * @param name the new name
          */
         void SetName(std::string name);

         /**
          * Returns the name of this instance.
          *
          * @return the current name
          */
         std::string GetName() const;

         void SetId( const Id& id ) { mId = id; };
         const Id& GetId() const { return mId; }

         ///Override to receive messages
         virtual void OnMessage(MessageData *data) {}

         ///Receive all messages from the supplied sender instance
         void AddSender( Base *sender );

         ///Stop receiving messages from the supplied sender instance
         void RemoveSender( Base *sender );
         
         /** Send a message to any instances that are subscribed
          *  to this instance.  Any supplied string or void* data will be passed
          *  to the receiver's OnMessage() method.
          *  @param message Optional string message
          *  @param data Optional pointer to user data
          */
         void SendMessage(std::string message="", void *data=NULL);
   };
};


#endif // DELTA_BASE
