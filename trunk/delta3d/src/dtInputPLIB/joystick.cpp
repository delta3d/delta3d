// joystick.cpp: Implementation of the Joystick class.
//
//////////////////////////////////////////////////////////////////////
#include <dtInputPLIB/joystick.h>
#include <plib/js.h>

#include <iostream>
#include <sstream>

using namespace dtInputPLIB;

IMPLEMENT_MANAGEMENT_LAYER(Joystick)


//////////////////////////////////////////////////////
void Joystick::CreateInstances()
{
   jsInit();
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
#ifndef __APPLE__
         delete joystick;
#endif
         keepGoing = false;
      }
   }
}

//////////////////////////////////////////////////////
void Joystick::DestroyInstances()
{
   while (GetInstanceCount()> 0)
   {
      delete GetInstance(0);
   }
}

//////////////////////////////////////////////////////
void Joystick::PollInstances()
{
   for (int i=0; i<GetInstanceCount();i++)
   {
      GetInstance(i)->Poll();
   }
}

//////////////////////////////////////////////////////
Joystick::Joystick(std::string name, jsJoystick* joystick) :
   InputDevice(name), mJoystick(joystick)
{
   RegisterInstance(this);

#if defined(PLIB_MAJOR_VERSION) && PLIB_MAJOR_VERSION >= 1 && defined(PLIB_MINOR_VERSION) & PLIB_MINOR_VERSION >= 8 && defined(PLIB_TINY_VERSION) && PLIB_TINY_VERSION >=5
   const unsigned int numButtons = mJoystick->getNumButtons();
#else
   const unsigned int numButtons = _JS_MAX_BUTTONS;
#endif // PLIB_MAJOR_VERSION

   for (unsigned int i=0; i<numButtons; i++)
   {
      std::ostringstream buf;

      buf << GetName()<< " button " << i;

      AddFeature(new dtCore::Button(this, i, buf.str()));
   }

   for (int j=0; j<mJoystick->getNumAxes();j++)
   {
      std::ostringstream buf;

      buf << GetName()<< " axis " << j;

      AddFeature(new dtCore::Axis(this, buf.str()));
   }
}

//////////////////////////////////////////////////////
Joystick::~Joystick()
{
   DeregisterInstance(this);

   delete mJoystick;
}

//////////////////////////////////////////////////////
const std::string Joystick::GetDeviceName() const
{
   return mJoystick->getName();
}

//////////////////////////////////////////////////////
void Joystick::Poll()
{
   const unsigned int numAxes = mJoystick->getNumAxes();

#if defined(PLIB_MAJOR_VERSION) && PLIB_MAJOR_VERSION >= 1 && defined(PLIB_MINOR_VERSION) & PLIB_MINOR_VERSION >= 8 && defined(PLIB_TINY_VERSION) && PLIB_TINY_VERSION >=5
   const unsigned int numButtons = mJoystick->getNumButtons();
#else
   const unsigned int numButtons = _JS_MAX_BUTTONS;
#endif // PLIB_MAJOR_VERSION

   int buttons;
   float *axes = new float[numAxes];

   mJoystick->read(&buttons, axes);

   for (unsigned int i = 0; i < numButtons; i++)
   {
      GetButton(i)->SetState((buttons & (1 << i)) != 0);
   }

   for (unsigned int j = 0; j < numAxes; j++)
   {
      GetAxis(j)->SetState(axes[j]);
   }

   delete axes;
}
