// logicalinputdevice.cpp: Implementation of the LogicalInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "logicalinputdevice.h"
#include "notify.h"

using namespace dtCore;
using namespace std;
using namespace osg;

IMPLEMENT_MANAGEMENT_LAYER(LogicalInputDevice)


/**
 * Constructor.
 *
 * @param name the instance name
 */
LogicalInputDevice::LogicalInputDevice(string name) : InputDevice(name)
{
   RegisterInstance(this);
}

/**
 * Destructor.
 */
LogicalInputDevice::~LogicalInputDevice()
{
   DeregisterInstance(this);
}

/**
 * Adds a new logical button to this device.
 *
 * @param description a description of the button
 * @param mapping the initial button mapping, or NULL for none
 */
LogicalButton* LogicalInputDevice::AddButton(string description,
                                             ButtonMapping* mapping)
{
   LogicalButton* button = new LogicalButton(this, description, mapping);

   AddFeature(button);

   return button;
}

/**
 * Adds a new logical button to this device.  Equivalent to AddButton(description,
 * new ButtonToButton(sourceButton)).
 *
 * @param description a description of the button
 * @param sourceButton the source button
 */
LogicalButton* LogicalInputDevice::AddButton(string description,
                                             Button* sourceButton)
{
   return AddButton(description, new ButtonToButton(sourceButton));
}

/**
 * Removes a logical button from this device.
 *
 * @param button the button to remove
 */
void LogicalInputDevice::RemoveButton(LogicalButton* button)
{
   RemoveFeature(button);
}
         
/**
 * Adds a new logical axis.
 *
 * @param description a description of the axis
 * @param mapping the initial axis mapping, or NULL for none
 */
LogicalAxis* LogicalInputDevice::AddAxis(string description,
                                         AxisMapping* mapping)
{
   LogicalAxis* axis = new LogicalAxis(this, description, mapping);

   AddFeature(axis);

   return axis;
}

/**
 * Adds a new logical axis.  Equivalent to AddAxis(description,
 * new AxisToAxis(sourceAxis)).
 *
 * @param description a description of the axis
 * @param sourceAxis the source axis
 */
LogicalAxis* LogicalInputDevice::AddAxis(string description,
                                         Axis* sourceAxis)
{
   return AddAxis(description, new AxisToAxis(sourceAxis));
}

/**
 * Removes a logical axis from this device.
 *
 * @param axis the axis to remove
 */
void LogicalInputDevice::RemoveAxis(LogicalAxis* axis)
{
   RemoveFeature(axis);
}

/**
 * Constructor.
 *
 * @param owner the owner of this button
 * @param description a description of this button
 * @param mapping the initial button mapping
 */
LogicalButton::LogicalButton(LogicalInputDevice* owner,
                             string description, 
                             ButtonMapping *mapping) :
   Button(owner, description),
   mMapping(mapping)
{
   if(mMapping.get() != NULL)
   {
      mMapping->SetTargetButton(this);
   }
}

/**
 * Sets this button's mapping.
 *
 * @param mapping the new mapping
 */
void LogicalButton::SetMapping(ButtonMapping* mapping)
{
   if(mMapping.get() != NULL)
   {
      mMapping->SetTargetButton(NULL);
   }

   mMapping = mapping;

   if(mMapping.get() != NULL)
   {
      mMapping->SetTargetButton(this);
   }
}

/**
 * Returns this button's mapping.
 *
 * @return the current mapping
 */
ButtonMapping* LogicalButton::GetMapping()
{
   return mMapping.get();
}

/**
 * Constructor.
 */
ButtonMapping::ButtonMapping()
{}

/**
 * Destructor.
 */
ButtonMapping::~ButtonMapping()
{}

/**
 * Constructor.
 *
 * @param sourceButton the source button
 */
ButtonToButton::ButtonToButton(Button* sourceButton) :
   mSourceButton(sourceButton),
   mTargetButton(NULL)
{}

/**
 * Destructor.
 */
ButtonToButton::~ButtonToButton()
{
   if(mSourceButton.get() != NULL)
   {
      mSourceButton->RemoveButtonListener(this);
   }
}

/**
 * Sets the source button.
 *
 * @param sourceButton the new source button
 */
void ButtonToButton::SetSourceButton(Button* sourceButton)
{
   if(mSourceButton.get() != NULL)
   {
      mSourceButton->RemoveButtonListener(this);
   }

   mSourceButton = sourceButton;

   if(mSourceButton.get() != NULL && mTargetButton.get() != NULL)
   {
      mSourceButton->AddButtonListener(this);
   }

   UpdateTargetButtonState();
}

/**
 * Returns the source button.
 *
 * @return the current source button
 */
Button* ButtonToButton::GetSourceButton()
{
   return mSourceButton.get();
}

/**
 * Sets the target button.
 *
 * @param targetButton the new target button
 */
void ButtonToButton::SetTargetButton(LogicalButton* targetButton)
{
   mTargetButton = targetButton;

   if(mSourceButton.get() != NULL)
   {
      if(mTargetButton.get() != NULL)
      {
         mSourceButton->AddButtonListener(this);
      }
      else
      {
         mSourceButton->RemoveButtonListener(this);
      }
   }

   UpdateTargetButtonState();
}

/**
 * Gets the target button.
 *
 * @return the current target button
 */
LogicalButton* ButtonToButton::GetTargetButton()
{
   return mTargetButton.get();
}

/**
 * Called when a button's state has changed.
 *
 * @param button the origin of the event
 * @param oldState the old state of the button
 * @param newState the new state of the button
 */
void ButtonToButton::ButtonStateChanged(Button* button,
                                        bool oldState,
                                        bool newState)
{
   if(mTargetButton.get() != NULL)
   {
      mTargetButton->SetState(newState);
   }
}

/**
 * Updates the state of the target button.
 */
void ButtonToButton::UpdateTargetButtonState()
{
   if(mTargetButton.get() != NULL)
   {
      bool state = false;

      if(mSourceButton.get() != NULL)
      {
         state = mSourceButton->GetState();
      }

      mTargetButton->SetState(state);
   }
}

/**
 * Constructor.
 *
 * @param owner the owner of this axis
 * @param description a description of this axis
 * @param mapping the initial mapping
 */
LogicalAxis::LogicalAxis(LogicalInputDevice* owner,
                         string description,
                         AxisMapping* mapping) :
   Axis(owner, description),
   mMapping(mapping)
{
   if(mMapping.get() != NULL)
   {
      mMapping->SetTargetAxis(this);
   }
}

/**
 * Sets this axis' mapping.
 *
 * @param mapping the new mapping
 */
void LogicalAxis::SetMapping(AxisMapping* mapping)
{
   if(mMapping.get() != NULL)
   {
      mMapping->SetTargetAxis(NULL);
   }

   mMapping = mapping;

   if(mMapping.get() != NULL)
   {
      mMapping->SetTargetAxis(this);
   }
}

/**
 * Returns this axis' mapping.
 *
 * @return the current mapping
 */
AxisMapping* LogicalAxis::GetMapping()
{
   return mMapping.get();
}

/**
 * Constructor.
 */
AxisMapping::AxisMapping()
{}

/**
 * Destructor.
 */
AxisMapping::~AxisMapping()
{}

/**
 * Constructor.
 *
 * @param sourceAxis the source axis
 * @param scale the optional scale parameter
 * @param offset the optional offset parameter
 */
AxisToAxis::AxisToAxis(Axis* sourceAxis, double scale, double offset)
   : mSourceAxis(sourceAxis),
     mTargetAxis(NULL),
     mScale(scale),
     mOffset(offset)
{}

/**
 * Destructor.
 */
AxisToAxis::~AxisToAxis()
{
   if(mSourceAxis.get() != NULL)
   {
      mSourceAxis->RemoveAxisListener(this);
   }
}

/**
 * Sets the source axis.
 *
 * @param sourceAxis the new source axis
 */
void AxisToAxis::SetSourceAxis(Axis* sourceAxis)
{
   if(mSourceAxis.get() != NULL)
   {
      mSourceAxis->RemoveAxisListener(this);
   }

   mSourceAxis = sourceAxis;

   if(mSourceAxis.get() != NULL && mTargetAxis.get() != NULL)
   {
      mSourceAxis->AddAxisListener(this);
   }

   UpdateTargetAxisState();
}

/**
 * Returns the source axis.
 *
 * @return the current source axis
 */
Axis* AxisToAxis::GetSourceAxis()
{
   return mSourceAxis.get();
}

/**
 * Sets the target axis.
 *
 * @param targetAxis the new target axis
 */
void AxisToAxis::SetTargetAxis(LogicalAxis* targetAxis)
{
   mTargetAxis = targetAxis;

   if(mSourceAxis.get() != NULL && mTargetAxis.get() != NULL)
   {
      mSourceAxis->AddAxisListener(this);
   }
   else
   {
      mSourceAxis->RemoveAxisListener(this);
   }

   UpdateTargetAxisState();
}

/**
 * Gets the target axis.
 *
 * @return the current target axis
 */
LogicalAxis* AxisToAxis::GetTargetAxis()
{
   return mTargetAxis.get();
}

/**
 * Sets this mapping's linear transformation parameters.
 *
 * @param scale the new scale parameter
 * @param offset the new offset parameter
 */
void AxisToAxis::SetTransformationParameters(double scale, double offset)
{
   mScale = scale;
   mOffset = offset;

   UpdateTargetAxisState();
}

/**
 * Retrieves this mapping's linear transformation parameters.
 *
 * @param scale a pointer to the location in which to store the scale
 * parameter
 * @param offset a pointer to the location in which to store the offset
 * parameter
 */
void AxisToAxis::GetTransformationParameters(double* scale, double* offset) const
{
   (*scale) = mScale;
   (*offset) = mOffset;
}

/**
 * Called when an axis' state has changed.
 *
 * @param axis the changed axis
 * @param oldState the old state of the axis
 * @param newState the new state of the axis
 * @param delta a delta value indicating stateless motion
 */
void AxisToAxis::AxisStateChanged(Axis* axis,
                                  double oldState, 
                                  double newState, 
                                  double delta)
{
   if(mTargetAxis.get() != NULL)
   {
      mTargetAxis->SetState(newState*mScale + mOffset, delta*mScale);
   }
}

/**
 * Updates the state of the target axis.
 */
void AxisToAxis::UpdateTargetAxisState()
{
   if(mTargetAxis.get() != NULL)
   {
      double value = 0.0;

      if(mSourceAxis.get() != NULL)
      {
         value = mSourceAxis->GetState();
      }

      mTargetAxis->SetState(value*mScale + mOffset);
   }
}


/**
 * Constructor.
 *
 * @param firstSourceAxis the first source axis, or NULL for none
 * @param secondSourceAxis the second source axis, or NULL for none
 */
AxesToAxis::AxesToAxis(Axis* firstSourceAxis,
                       Axis* secondSourceAxis)
   : mTargetAxis(NULL)
{
   if(firstSourceAxis != NULL)
   {
      AddSourceAxis(firstSourceAxis);
   }
   
   if(secondSourceAxis != NULL)
   {
      AddSourceAxis(secondSourceAxis);
   }
}

/**
 * Destructor.
 */
AxesToAxis::~AxesToAxis()
{
   for(vector< ref_ptr<Axis> >::iterator it = mSourceAxes.begin();
       it != mSourceAxes.end();
       it++)
   {
      (*it)->RemoveAxisListener(this);
   }
}

/**
 * Adds a source axis.
 *
 * @param sourceAxis the source axis to add
 */
void AxesToAxis::AddSourceAxis(Axis* sourceAxis)
{
   sourceAxis->AddAxisListener(this);
   
   mSourceAxes.push_back(sourceAxis);
   
   UpdateTargetAxisState();
}

/**
 * Removes a source axis.
 *
 * @param sourceAxis the source axis to remove
 */
void AxesToAxis::RemoveSourceAxis(Axis* sourceAxis)
{
   for(vector< ref_ptr<Axis> >::iterator it = mSourceAxes.begin();
       it != mSourceAxes.end();
       it++)
   {
      if((*it).get() == sourceAxis)
      {
         sourceAxis->RemoveAxisListener(this);
         
         mSourceAxes.erase(it);
         
         return;
      }
   }
   
   UpdateTargetAxisState();
}

/**
 * Returns the number of source axes.
 *
 * @return the number of source axes
 */
int AxesToAxis::GetNumSourceAxes()
{
   return mSourceAxes.size();
}

/**
 * Returns the source axis at the specified index.
 *
 * @param index the index of the axis to retrieve
 * @return the axis at the specified index
 */
Axis* AxesToAxis::GetSourceAxis(int index)
{
   return mSourceAxes[index].get();
}

/**
 * Sets the target axis.
 *
 * @param targetAxis the new target axis
 */
void AxesToAxis::SetTargetAxis(LogicalAxis* targetAxis)
{
   mTargetAxis = targetAxis;
   
   UpdateTargetAxisState();
}

/**
 * Gets the target axis.
 *
 * @return the current target axis
 */
LogicalAxis* AxesToAxis::GetTargetAxis()
{
   return mTargetAxis.get();
}

/**
 * Called when an axis' state has changed.
 *
 * @param axis the changed axis
 * @param oldState the old state of the axis
 * @param newState the new state of the axis
 * @param delta a delta value indicating stateless motion
 */
void AxesToAxis::AxisStateChanged(Axis* axis,
                                  double oldState, 
                                  double newState, 
                                  double delta)
{
   if(mTargetAxis.get() != NULL)
   {
      mTargetAxis->SetState(newState, delta);
   }
}

/**
 * Updates the state of the target axis.
 */
void AxesToAxis::UpdateTargetAxisState()
{
   if(mTargetAxis.get() != NULL)
   {
      if(mSourceAxes.size() == 0)
      {
         mTargetAxis->SetState(
            mSourceAxes[mSourceAxes.size()-1]->GetState()
         );
      }
      else
      {
         mTargetAxis->SetState(0.0f);
      }
   }
}

                                   
/**
 * Constructor.
 *
 * @param firstSourceButton the first source button
 * @param secondSourceButton the second source button
 * @param firstButtonValue the value corresponding to the first
 * source button
 * @param secondButtonValue the value corresponding to the second
 * source button
 * @param neutralValue the value corresponding to the neutral
 * state
 */
ButtonsToAxis::ButtonsToAxis(Button* firstSourceButton,
                             Button* secondSourceButton,
                             double firstButtonValue,
                             double secondButtonValue,
                             double neutralValue)
   : mFirstSourceButton(firstSourceButton),
     mSecondSourceButton(secondSourceButton),
     mFirstButtonValue(firstButtonValue),
     mSecondButtonValue(secondButtonValue),
     mNeutralValue(neutralValue)
{}

/**
 * Destructor.
 */
ButtonsToAxis::~ButtonsToAxis()
{
   if(mFirstSourceButton.get() != NULL)
   {
      mFirstSourceButton->RemoveButtonListener(this);
   }

   if(mSecondSourceButton.get() != NULL)
   {
      mSecondSourceButton->RemoveButtonListener(this);
   }
}

/**
 * Sets the two source buttons.
 *
 * @param firstButton the first button
 * @param secondButton the second button
 */
void ButtonsToAxis::SetSourceButtons(Button* firstSourceButton,
                                     Button* secondSourceButton)
{
   if(mFirstSourceButton.get() != NULL)
   {
      mFirstSourceButton->RemoveButtonListener(this);
   }

   if(mSecondSourceButton.get() != NULL)
   {
      mSecondSourceButton->RemoveButtonListener(this);
   }

   mFirstSourceButton = firstSourceButton;
   mSecondSourceButton = secondSourceButton;

   if(mTargetAxis.get() != NULL)
   {
      if(mFirstSourceButton.get() != NULL)
      {
         mFirstSourceButton->AddButtonListener(this);
      }

      if(mSecondSourceButton.get() != NULL)
      {
         mSecondSourceButton->AddButtonListener(this);
      }
   }

   UpdateTargetAxisState();
}

/**
 * Retrieves pointers to the two source buttons.
 *
 * @param firstButton a pointer to the location in which to store a
 * pointer to the first button
 * @param secondButton a pointer to the location in which to store a
 * pointer to the second button
 */
void ButtonsToAxis::GetSourceButtons(Button** firstSourceButton,
                                     Button** secondSourceButton)
{
   (*firstSourceButton) = mFirstSourceButton.get();
   (*secondSourceButton) = mSecondSourceButton.get();
}

/**
 * Sets the target axis.
 *
 * @param targetAxis the new target axis
 */
void ButtonsToAxis::SetTargetAxis(LogicalAxis* targetAxis)
{
   mTargetAxis = targetAxis;

   if(mTargetAxis.get() != NULL)
   {
      if(mFirstSourceButton.get() != NULL)
      {
         mFirstSourceButton->AddButtonListener(this);
      }

      if(mSecondSourceButton.get() != NULL)
      {
         mSecondSourceButton->AddButtonListener(this);
      }
   }
   else
   {
      if(mFirstSourceButton.get() != NULL)
      {
         mFirstSourceButton->RemoveButtonListener(this);
      }

      if(mSecondSourceButton.get() != NULL)
      {
         mSecondSourceButton->RemoveButtonListener(this);
      }
   }

   UpdateTargetAxisState();
}

/**
 * Gets the target axis.
 *
 * @return the current target axis
 */
LogicalAxis* ButtonsToAxis::GetTargetAxis()
{
   return mTargetAxis.get();
}

/**
 * Sets the axis values.
 *
 * @param firstButtonValue the value corresponding to the first button
 * @param secondButtonValue the value corresponding to the second button
 * @param neutralValue the value corresponding to the neutral state
 */
void ButtonsToAxis::SetValues(double firstButtonValue,
                              double secondButtonValue,
                              double neutralValue)
{
   mFirstButtonValue = firstButtonValue;
   mSecondButtonValue = secondButtonValue;
   mNeutralValue = neutralValue;

   UpdateTargetAxisState();
}

/**
 * Retrieves the axis values.
 *
 * @param firstButtonValue a pointer to the location in which to store
 * the value corresponding to the first button
 * @param secondButtonValue a pointer to the location in which to store
 * the value corresponding to the second button
 * @param neutralValue a pointer to the location in which to store
 * the value corresponding to the neutral state
 */
void ButtonsToAxis::GetValues(double* firstButtonValue,
                              double* secondButtonValue, 
                              double* neutralValue) const
{
   (*firstButtonValue) = mFirstButtonValue;
   (*secondButtonValue) = mSecondButtonValue;
   (*neutralValue) = mNeutralValue;
}

/**
 * Called when a button's state has changed.
 *
 * @param button the origin of the event
 * @param oldState the old state of the button
 * @param newState the new state of the button
 */
void ButtonsToAxis::ButtonStateChanged(Button* button,
                                       bool oldState,
                                       bool newState)
{
   UpdateTargetAxisState();
}

/**
 * Updates the state of the target axis.
 */
void ButtonsToAxis::UpdateTargetAxisState()
{
   if(mTargetAxis.get() != NULL)
   {
      bool firstButtonState = false,
           secondButtonState = false;

      if(mFirstSourceButton.get() != NULL)
      {
         firstButtonState = mFirstSourceButton->GetState();
      }

      if(mSecondSourceButton.get() != NULL)
      {
         secondButtonState = mSecondSourceButton->GetState();
      }

      if(firstButtonState && !secondButtonState)
      {
         mTargetAxis->SetState(mFirstButtonValue);
      }
      else if(secondButtonState && !firstButtonState)
      {
         mTargetAxis->SetState(mSecondButtonValue);
      }
      else
      {
         mTargetAxis->SetState(mNeutralValue);
      }
   }
}


/**
 * Constructor.
 *
 * @param sourceButton the source button
 * @param sourceAxis the source axis
 */
ButtonAxisToAxis::ButtonAxisToAxis(Button* sourceButton, Axis* sourceAxis)
   : mSourceButton(sourceButton),
     mSourceAxis(sourceAxis)
{}

/**
 * Destructor.
 */
ButtonAxisToAxis::~ButtonAxisToAxis()
{
   if(mSourceButton.get() != NULL)
   {
      mSourceButton->RemoveButtonListener(this);
   }

   if(mSourceAxis.get() != NULL)
   {
      mSourceAxis->RemoveAxisListener(this);
   }
}

/**
 * Sets the source button.
 *
 * @param sourceButton the new source button
 */
void ButtonAxisToAxis::SetSourceButton(Button* sourceButton)
{
   if(mSourceButton.get() != NULL)
   {
      mSourceButton->RemoveButtonListener(this);
   }
   
   mSourceButton = sourceButton;
   
   if(mSourceButton.get() != NULL)
   {
      mSourceButton->AddButtonListener(this);
   }
   
   UpdateTargetAxisState();
}

/**
 * Returns the source button.
 *
 * @return the current source button
 */
Button* ButtonAxisToAxis::GetSourceButton()
{
   return mSourceButton.get();
}

/**
 * Sets the source axis.
 *
 * @param sourceAxis the new source axis
 */
void ButtonAxisToAxis::SetSourceAxis(Axis* sourceAxis)
{
   if(mSourceAxis.get() != NULL)
   {
      mSourceAxis->RemoveAxisListener(this);
   }
   
   mSourceAxis = sourceAxis;
   
   if(mSourceAxis.get() != NULL)
   {
      mSourceAxis->AddAxisListener(this);
   }
   
   UpdateTargetAxisState();
}

/**
 * Returns the source axis.
 *
 * @return the current source axis.
 */
Axis* ButtonAxisToAxis::GetSourceAxis()
{
   return mSourceAxis.get();
}

/**
 * Sets the target axis.
 *
 * @param targetAxis the new target axis
 */
void ButtonAxisToAxis::SetTargetAxis(LogicalAxis* targetAxis)
{
   mTargetAxis = targetAxis;
   
   if(mTargetAxis.get() != NULL)
   {
      if(mSourceButton.get() != NULL)
      {
         mSourceButton->AddButtonListener(this);
      }

      if(mSourceAxis.get() != NULL)
      {
         mSourceAxis->AddAxisListener(this);
      }
   }
   else
   {
      if(mSourceButton.get() != NULL)
      {
         mSourceButton->RemoveButtonListener(this);
      }

      if(mSourceAxis.get() != NULL)
      {
         mSourceAxis->RemoveAxisListener(this);
      }
   }

   UpdateTargetAxisState();
}

/**
 * Gets the target axis.
 *
 * @return the current target axis
 */
LogicalAxis* ButtonAxisToAxis::GetTargetAxis()
{
   return mTargetAxis.get();
}

/**
 * Called when a button's state has changed.
 *
 * @param button the origin of the event
 * @param oldState the old state of the button
 * @param newState the new state of the button
 */
void ButtonAxisToAxis::ButtonStateChanged(Button* button,
                                          bool oldState,
                                          bool newState)
{
   UpdateTargetAxisState();
}

/**
 * Called when an axis' state has changed.
 *
 * @param axis the changed axis
 * @param oldState the old state of the axis
 * @param newState the new state of the axis
 * @param delta a delta value indicating stateless motion
 */
void ButtonAxisToAxis::AxisStateChanged(Axis* axis,
                                        double oldState, 
                                        double newState, 
                                        double delta)
{
   if(mTargetAxis.get() != NULL &&
      mSourceButton.get() != NULL &&
      mSourceButton->GetState())
   {
      mTargetAxis->SetState(newState, delta);
   }
}

/**
 * Updates the state of the target axis.
 */
void ButtonAxisToAxis::UpdateTargetAxisState()
{
   if(mTargetAxis.get() != NULL)
   {
      if(mSourceButton.get() != NULL &&
         mSourceAxis.get() != NULL &&
         mSourceButton->GetState())
      {
         mTargetAxis->SetState(mSourceAxis->GetState());
      }
      else
      {
         mTargetAxis->SetState(0.0f);
      }
   }
}
