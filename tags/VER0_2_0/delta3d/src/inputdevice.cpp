// inputdevice.cpp: Implementation of the InputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "inputdevice.h"

using namespace dtCore;
using namespace std;
using namespace osg;

IMPLEMENT_MANAGEMENT_LAYER(InputDevice)


/**
 * Constructor.
 *
 * @param name the instance name
 */
InputDevice::InputDevice(string name) : Base(name)
{
   RegisterInstance(this);
}

/**
 * Destructor.
 */
InputDevice::~InputDevice()
{
   DeregisterInstance(this);
}

/**
 * Returns the number of features (buttons, axes, etc.) contained in 
 * this device.
 *
 * @return the number of features contained in this device
 */
int InputDevice::GetFeatureCount() const
{
   return mFeatures.size();
}

/**
 * Returns a pointer to the specified feature.
 *
 * @param index the index of the desired feature
 * @return a pointer to the feature
 */
InputDeviceFeature* InputDevice::GetFeature(int index)
{
   return mFeatures[index].get();
}

/**
 * Returns the number of features (buttons, axes, etc.) contained in 
 * this device.
 *
 * @return the number of features contained in this device
 */
int InputDevice::GetButtonCount() const
{
   return mButtons.size();
}

/**
 * Returns a pointer to the specified feature.
 *
 * @param index the index of the desired feature
 * @return a pointer to the feature
 */
Button* InputDevice::GetButton(int index)
{
   return mButtons[index].get();
}

/**
 * Returns the number of features (buttons, axes, etc.) contained in 
 * this device.
 *
 * @return the number of features contained in this device
 */
int InputDevice::GetAxisCount() const
{
   return mAxes.size();
}

/**
 * Returns a pointer to the specified feature.
 *
 * @param index the index of the desired feature
 * @return a pointer to the feature
 */
Axis* InputDevice::GetAxis(int index)
{
   return mAxes[index].get();
}

/**
 * Adds a button listener.
 *
 * @param buttonListener a pointer to the listener to add
 */
void InputDevice::AddButtonListener(ButtonListener* buttonListener)
{
   mButtonListeners.insert(buttonListener);
}

/**
 * Removes a button listener.
 *
 * @param buttonListener a pointer to the listener to remove
 */
void InputDevice::RemoveButtonListener(ButtonListener* buttonListener)
{
   mButtonListeners.erase(buttonListener);
}

/**
 * Adds an axis listener.
 *
 * @param axisListener a pointer to the listener to add
 */
void InputDevice::AddAxisListener(AxisListener* axisListener)
{
   mAxisListeners.insert(axisListener);
}

/**
 * Removes an axis listener.
 *
 * @param axisListener a pointer to the listener to remove
 */
void InputDevice::RemoveAxisListener(AxisListener* axisListener)
{
   mAxisListeners.erase(axisListener);
}

/**
 * Adds a feature to this device.
 *
 * @param feature a pointer to the feature to add
 */
void InputDevice::AddFeature(InputDeviceFeature* feature)
{
   mFeatures.push_back(feature);

   if(IS_A(feature, Button*))
   {
      mButtons.push_back((Button*)feature);
   }
   
   if(IS_A(feature, Axis*))
   {
      mAxes.push_back((Axis*)feature);
   }
}

/**
* Removes a feature from this device.
*
* @param feature a pointer to the feature to remove
*/
void InputDevice::RemoveFeature(InputDeviceFeature* feature)
{
   for(vector< ref_ptr<InputDeviceFeature> >::iterator it = mFeatures.begin();
       it != mFeatures.end();
       it++)
   {
      if((*it).get() == feature)
      {
         mFeatures.erase(it);

         if(IS_A(feature, Button*))
         {
            for(vector< ref_ptr<Button> >::iterator bit = mButtons.begin();
                bit != mButtons.end();
                bit++)
            {
               if((*bit).get() == feature)
               {
                  mButtons.erase(bit);
                  break;
               }
            }
         }

         if(IS_A(feature, Axis*))
         {
            for(vector< ref_ptr<Axis> >::iterator ait = mAxes.begin();
                ait != mAxes.end();
                ait++)
            {
               if((*ait).get() == feature)
               {
                  mAxes.erase(ait);
                  break;
               }
            }
         }

         return;
      }
   }
}

/**
 * Constructor.
 *
 * @param owner the owner of this feature
 * @param description a description of this feature
 */
InputDeviceFeature::InputDeviceFeature(InputDevice* owner, string description) :
   mOwner(owner),
   mDescription(description)
{}

/**
 * Destructor.
 */
InputDeviceFeature::~InputDeviceFeature()
{}

/**
 * Returns a pointer to the owner of this feature.
 *
 * @return a pointer to the owner of this feature
 */
InputDevice* InputDeviceFeature::GetOwner() const
{
   return mOwner;
}

/**
 * Sets the description of this feature.
 *
 * @param description the new description
 */
void InputDeviceFeature::SetDescription(string description)
{
   mDescription = description;
}

/**
 * Returns a description of this feature.
 *
 * @return a description of this feature
 */
string InputDeviceFeature::GetDescription() const
{
   return mDescription;
}

/**
 * Constructor.
 *
 * @param owner the owner of this button
 * @param description a description of this button
 */
Button::Button(InputDevice* owner, string description) :
   InputDeviceFeature(owner, description),
   mState(false)
{}

/**
 * Sets the state of this button.
 *
 * @param state the new state
 */
void Button::SetState(bool state)
{
   if(state != mState)
   {
      mState = state;

      set<ButtonListener*>::iterator it;
      
      for(it = mButtonListeners.begin();
          it != mButtonListeners.end();
          it++)
      {
         (*it)->ButtonStateChanged(this, !mState, mState);
      }
      
      for(it = GetOwner()->mButtonListeners.begin();
          it != GetOwner()->mButtonListeners.end();
          it++)
      {
         (*it)->ButtonStateChanged(this, !mState, mState);
      }
   }
}

/**
 * Returns the current state of this button.
 *
 * @return true if this button is pressed, false
 * otherwise
 */
bool Button::GetState() const
{
   return mState;
}

/**
 * Adds a button listener.
 *
 * @param buttonListener a pointer to the listener to add
 */
void Button::AddButtonListener(ButtonListener* buttonListener)
{
   mButtonListeners.insert(buttonListener);
}

/**
 * Removes a button listener.
 *
 * @param buttonListener a pointer to the listener to remove
 */
void Button::RemoveButtonListener(ButtonListener* buttonListener)
{
   mButtonListeners.erase(buttonListener);
}

/**
 * Constructor.
 *
 * @param owner the owner of this axis
 * @param description a description of this axis
 */
Axis::Axis(InputDevice* owner, string description) : 
   InputDeviceFeature(owner, description),
   mState(0.0)
{}

/**
 * Sets the state of this axis.  The delta value is used to indicate
 * stateless motion, such as mouse scrolling.
 *
 * @param axis the new state
 * @param delta the optional delta value
 */
void Axis::SetState(double state, double delta)
{
   if(state != mState || delta != 0.0)
   {
      double oldState = mState;

      mState = state;

      set<AxisListener*>::iterator it;
      
      for(it = mAxisListeners.begin();
          it != mAxisListeners.end();
          it++)
      {
         (*it)->AxisStateChanged(this, oldState, mState, delta);
      }
      
      for(it = GetOwner()->mAxisListeners.begin();
          it != GetOwner()->mAxisListeners.end();
          it++)
      {
         (*it)->AxisStateChanged(this, oldState, mState, delta);
      }
   }
}

/**
 * Returns the current state of this axis.
 *
 * @return the current state of this axis
 */
double Axis::GetState() const
{
   return mState;
}

/**
 * Adds an axis listener.
 *
 * @param axisListener a pointer to the listener to add
 */
void Axis::AddAxisListener(AxisListener* axisListener)
{
   mAxisListeners.insert(axisListener);
}

/**
 * Removes an axis listener.
 *
 * @param axisListener a pointer to the listener to remove
 */
void Axis::RemoveAxisListener(AxisListener* axisListener)
{
   mAxisListeners.erase(axisListener);
}
