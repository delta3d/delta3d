// joystick.cpp: Implementation of the Joystick class.
//
//////////////////////////////////////////////////////////////////////
#include <dtInputPLIB/joystick.h>

#include <iostream>
#include <sstream>

using namespace dtInputPLIB;

IMPLEMENT_MANAGEMENT_LAYER(Joystick)

/**
 * The maximum number of joystick buttons.
 */
const int maxJoystickButtons = 16;

/**
 * Creates instances of Joystick corresponding to each
 * connected joystick device.
 */
void Joystick::CreateInstances()
{
   bool keepGoing = true;
   jsJoystick* joystick;

   for (int i=0; keepGoing;i++)
   {
      joystick = new jsJoystick(i);

      if (joystick->notWorking()== JS_FALSE)
      {
         std::ostringstream buf;

         buf << "joystick " << i;

         new Joystick(buf.str(), joystick);
      }
      else
      {
         delete joystick;

         keepGoing = false;
      }
   }
}

/**
 * Destroys all Joystick instances.
 */
void Joystick::DestroyInstances()
{
   while (GetInstanceCount()> 0)
   {
      delete GetInstance(0);
   }
}

/**
 * Polls all Joystick instances.
 */
void Joystick::PollInstances()
{
   for (int i=0; i<GetInstanceCount();i++)
   {
      GetInstance(i)->Poll();
   }
}

/**
 * Constructor.
 *
 * @param name the instance name
 * @param joystick the underlying PLIB joystick object
 */
Joystick::Joystick(std::string name, jsJoystick* joystick) :
   InputDevice(name), mJoystick(joystick)
{
   RegisterInstance(this);

   for (int i=0; i<maxJoystickButtons;i++)
   {
      std::ostringstream buf;

      buf << GetName()<< " button " << i;

      AddFeature(new dtCore::Button(this, buf.str()));
   }

   for (int j=0; j<mJoystick->getNumAxes();j++)
   {
      std::ostringstream buf;

      buf << GetName()<< " axis " << j;

      AddFeature(new dtCore::Axis(this, buf.str()));
   }
}

/**
 * Destructor.
 */
Joystick::~Joystick()
{
   DeregisterInstance(this);

   delete mJoystick;
}

/**
 * Manually polls the state of this joystick, updating
 * all of its features.
 */
void Joystick::Poll()
{
   int buttons, numAxes = mJoystick->getNumAxes();
   float *axes = new float[numAxes];

   mJoystick->read(&buttons, axes);

   for (int i=0; i<maxJoystickButtons;i++)
   {
      GetButton(i)->SetState((buttons & (1 << i)) != 0);
   }

   for (int j=0; j<numAxes;j++)
   {
      GetAxis(j)->SetState(axes[j]);
   }

   delete axes;
}
