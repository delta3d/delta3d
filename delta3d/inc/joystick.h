// joystick.h: Declaration of the Joystick class.
//
//////////////////////////////////////////////////////////////////////

#ifndef JOYSTICK
#define JOYSTICK

#include <string>

#include "js.h"

#include "inputdevice.h"


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

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1

#define _AUTOLIBNAME "winmm.lib"

#if defined(_DEBUG)
   #define _AUTOLIBNAME1 "js_d.lib"
#else
   #define _AUTOLIBNAME1 "js.lib"
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#pragma message( "Will automatically link with " _AUTOLIBNAME1 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)

#endif // JOYSTICK
