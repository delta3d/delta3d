// inputdevicefeature.cpp: Implementation of the InputDeviceFeature class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/inputdevicefeature.h>
#include <dtCore/inputdevice.h>
#include <algorithm>

namespace dtCore
{
   InputDeviceFeature::InputDeviceFeature(InputDevice* owner, const std::string& description) :
      mOwner(owner),
      mDescription(description)
   {}

   InputDeviceFeature::~InputDeviceFeature()
   {
      mOwner = NULL;
   }

   InputDevice* InputDeviceFeature::GetOwner() const
   {
      return mOwner.get();
   }

   void InputDeviceFeature::SetDescription(const std::string& description)
   {
      mDescription = description;
   }

   std::string InputDeviceFeature::GetDescription() const
   {
      return mDescription;
   }
}
