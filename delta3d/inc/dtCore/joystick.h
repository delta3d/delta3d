#ifndef DELTA_JOYSTICK
#define DELTA_JOYSTICK

// joystick.h: Declaration of the Joystick class.
//
//////////////////////////////////////////////////////////////////////


#include <string>

#include "js.h"

#include "dtCore/inputdevice.h"


namespace dtCore
{
   /**
    * A joystick device.
    */
   class DT_EXPORT Joystick : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Joystick)


      public:

         /**
          * Creates instances of Joystick corresponding to each
          * connected joystick device.
          */
         static void CreateInstances();

         /**
          * Destroys all Joystick instances.
          */
         static void DestroyInstances();

         /**
          * Polls all Joystick instances.
          */
         static void PollInstances();

         /**
          * Manually polls the state of this joystick, updating
          * all of its features.
          */
         void Poll();


      private:

         /**
          * Constructor.
          *
          * @param name the instance name
          * @param joystick the underlying PLIB joystick object
          */
         Joystick(std::string name, jsJoystick* joystick);

         /**
          * Destructor.
          */
         virtual ~Joystick();

         /**
          * The underlying PLIB joystick object.
          */
         jsJoystick* mJoystick;
   };
};


#endif // DELTA_JOYSTICK
