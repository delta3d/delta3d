// logicalinputdevice.cpp: Implementation of the LogicalInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/logicalinputdevice.h"

namespace dtCore
{

   IMPLEMENT_MANAGEMENT_LAYER(LogicalInputDevice)
   
   /**
    * Constructor.
    *
    * @param name the instance name
    */
    LogicalInputDevice::LogicalInputDevice(const std::string& name) : InputDevice(name)
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
   LogicalButton* LogicalInputDevice::AddButton(const std::string& description,
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
   LogicalButton* LogicalInputDevice::AddButton(const std::string& description,
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
   LogicalAxis* LogicalInputDevice::AddAxis(const std::string& description,
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
   LogicalAxis* LogicalInputDevice::AddAxis(const std::string& description,
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
                                const std::string& description, 
                                ButtonMapping *mapping) :
      Button(owner, description),
      mMapping(mapping)
   {
      if(mMapping.valid())
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
      if(mMapping.valid())
      {
         mMapping->SetTargetButton(NULL);
      }
   
      mMapping = mapping;
   
      if(mMapping.valid())
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
      if(mSourceButton.valid())
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
      if(mSourceButton.valid())
      {
         mSourceButton->RemoveButtonListener(this);
      }
   
      mSourceButton = sourceButton;
   
      if(mSourceButton.valid() && mTargetButton.valid())
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
   
      if(mSourceButton.valid())
      {
         if(mTargetButton.valid())
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
   bool ButtonToButton::ButtonStateChanged(const Button* button, bool oldState, bool newState)
   {
      if(mTargetButton.valid())
      {
         return mTargetButton->SetState(newState);
      }

      return false;
   }
   
   /**
    * Updates the state of the target button.
    */
   void ButtonToButton::UpdateTargetButtonState()
   {
      if(mTargetButton.valid())
      {
         bool state = false;
   
         if(mSourceButton.valid())
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
                            const std::string& description,
                            AxisMapping* mapping) :
      Axis(owner, description),
      mMapping(mapping)
   {
      if(mMapping.valid())
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
      if(mMapping.valid())
      {
         mMapping->SetTargetAxis(NULL);
      }
   
      mMapping = mapping;
   
      if(mMapping.valid())
      {
         mMapping->SetTargetAxis(this);
      }
   }
   
   AxisMapping* LogicalAxis::GetMapping()
   {
      return mMapping.get();
   }
   
   AxisMapping::AxisMapping()
   {}
   
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
   
   AxisToAxis::~AxisToAxis()
   {
      if(mSourceAxis.valid())
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
      if(mSourceAxis.valid())
      {
         mSourceAxis->RemoveAxisListener(this);
      }
   
      mSourceAxis = sourceAxis;
   
      if(mSourceAxis.valid() && mTargetAxis.valid())
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
   
      if(mSourceAxis.valid() && mTargetAxis.valid())
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
   bool AxisToAxis::AxisStateChanged(const Axis* axis,
                                     double oldState, 
                                     double newState, 
                                     double delta)
   {
      if(mTargetAxis.valid())
      {
         return mTargetAxis->SetState(newState*mScale + mOffset, delta*mScale);
      }

      return false;
   }

   bool AxisToAxis::UpdateTargetAxisState()
   {
      if(mTargetAxis.valid())
      {
         double value = 0.0;
   
         if(mSourceAxis.valid())
         {
            value = mSourceAxis->GetState();
         }
   
         return mTargetAxis->SetState(value*mScale + mOffset);
      }

      return false;
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
   
   AxesToAxis::~AxesToAxis()
   {
      for(std::vector< RefPtr<Axis> >::iterator it = mSourceAxes.begin();
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
      for(std::vector< RefPtr<Axis> >::iterator it = mSourceAxes.begin();
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
   bool AxesToAxis::AxisStateChanged(const Axis* axis,
                                     double oldState, 
                                     double newState, 
                                     double delta)
   {
      if(mTargetAxis.valid())
      {
         return mTargetAxis->SetState(newState, delta);
      }

      return false;
   }
   
   bool AxesToAxis::UpdateTargetAxisState()
   {
      if(mTargetAxis.valid())
      {
         if(mSourceAxes.size() == 0)
         {
            return mTargetAxis->SetState( mSourceAxes[mSourceAxes.size()-1]->GetState() );
         }
         else
         {
            return mTargetAxis->SetState(0.0f);
         }
      }

      return false;
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
      if(mFirstSourceButton.valid())
      {
         mFirstSourceButton->RemoveButtonListener(this);
      }
   
      if(mSecondSourceButton.valid())
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
      if(mFirstSourceButton.valid())
      {
         mFirstSourceButton->RemoveButtonListener(this);
      }
   
      if(mSecondSourceButton.valid())
      {
         mSecondSourceButton->RemoveButtonListener(this);
      }
   
      mFirstSourceButton = firstSourceButton;
      mSecondSourceButton = secondSourceButton;
   
      if(mTargetAxis.valid())
      {
         if(mFirstSourceButton.valid())
         {
            mFirstSourceButton->AddButtonListener(this);
         }
   
         if(mSecondSourceButton.valid())
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
   
      if(mTargetAxis.valid())
      {
         if(mFirstSourceButton.valid())
         {
            mFirstSourceButton->AddButtonListener(this);
         }
   
         if(mSecondSourceButton.valid())
         {
            mSecondSourceButton->AddButtonListener(this);
         }
      }
      else
      {
         if(mFirstSourceButton.valid())
         {
            mFirstSourceButton->RemoveButtonListener(this);
         }
   
         if(mSecondSourceButton.valid())
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

   bool ButtonsToAxis::ButtonStateChanged(const Button* button, bool oldState, bool newState)
   {
      return UpdateTargetAxisState();
   }

   bool ButtonsToAxis::UpdateTargetAxisState()
   {
      if(mTargetAxis.valid())
      {
         bool firstButtonState = false,
              secondButtonState = false;
   
         if(mFirstSourceButton.valid())
         {
            firstButtonState = mFirstSourceButton->GetState();
         }
   
         if(mSecondSourceButton.valid())
         {
            secondButtonState = mSecondSourceButton->GetState();
         }
   
         if(firstButtonState && !secondButtonState)
         {
            return mTargetAxis->SetState(mFirstButtonValue);
         }
         else if(secondButtonState && !firstButtonState)
         {
            return mTargetAxis->SetState(mSecondButtonValue);
         }
         else
         {
            return mTargetAxis->SetState(mNeutralValue);
         }
      }

      return false;
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
   
   ButtonAxisToAxis::~ButtonAxisToAxis()
   {
      if(mSourceButton.valid())
      {
         mSourceButton->RemoveButtonListener(this);
      }
   
      if(mSourceAxis.valid())
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
      if(mSourceButton.valid())
      {
         mSourceButton->RemoveButtonListener(this);
      }
      
      mSourceButton = sourceButton;
      
      if(mSourceButton.valid())
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
      if(mSourceAxis.valid())
      {
         mSourceAxis->RemoveAxisListener(this);
      }
      
      mSourceAxis = sourceAxis;
      
      if(mSourceAxis.valid())
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
      
      if(mTargetAxis.valid())
      {
         if(mSourceButton.valid())
         {
            mSourceButton->AddButtonListener(this);
         }
   
         if(mSourceAxis.valid())
         {
            mSourceAxis->AddAxisListener(this);
         }
      }
      else
      {
         if(mSourceButton.valid())
         {
            mSourceButton->RemoveButtonListener(this);
         }
   
         if(mSourceAxis.valid())
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
   bool ButtonAxisToAxis::ButtonStateChanged(const Button* button,
                                             bool oldState,
                                             bool newState)
   {
      return UpdateTargetAxisState();
   }
   
   /**
    * Called when an axis' state has changed.
    *
    * @param axis the changed axis
    * @param oldState the old state of the axis
    * @param newState the new state of the axis
    * @param delta a delta value indicating stateless motion
    */
   bool ButtonAxisToAxis::AxisStateChanged(const Axis* axis,
                                           double oldState, 
                                           double newState, 
                                           double delta)
   {
      if(mTargetAxis.valid() &&
         mSourceButton.valid() &&
         mSourceButton->GetState())
      {
         return mTargetAxis->SetState(newState, delta);
      }

      return false;
   }
   
   bool ButtonAxisToAxis::UpdateTargetAxisState()
   {
      if(mTargetAxis.valid())
      {
         if(mSourceButton.valid() &&
            mSourceAxis.valid() &&
            mSourceButton->GetState())
         {
            return mTargetAxis->SetState(mSourceAxis->GetState());
         }
         else
         {
            return mTargetAxis->SetState(0.0f);
         }
      }

      return false;
   }

}   // end of namespace
