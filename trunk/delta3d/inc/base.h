#ifndef DELTA_BASE
#define DELTA_BASE

// base.h: The root of the inheritance hierarchy.
//
//////////////////////////////////////////////////////////////////////


#include <string>

#include "export.h"
#include "macros.h"

#include <osg/Referenced>

//disable the "identifier was truncated to '255' characters " message

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
