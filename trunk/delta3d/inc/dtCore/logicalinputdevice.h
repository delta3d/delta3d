/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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

#ifndef DELTA_LOGICALINPUTDEVICE
#define DELTA_LOGICALINPUTDEVICE

// logicalinputdevice.h: Declaration of the LogicalInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/inputdevice.h>
#include <dtCore/axis.h>
#include <dtCore/axishandler.h>
#include <dtCore/axisobserver.h>
#include <dtCore/button.h>
#include <dtCore/buttonhandler.h>
#include <dtCore/buttonobserver.h>

namespace dtCore
{
   class LogicalButton;
   class ButtonMapping;
   class ButtonToButton;
   class LogicalAxis;
   class AxisMapping;

   /**
    * A logical input device.
    */
   class DT_CORE_EXPORT LogicalInputDevice : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(LogicalInputDevice)

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         LogicalInputDevice(const std::string& name = "LogicalInputDevice");

         /**
         * Adds a new logical button to this device.
         *
         * @param description a description of the button
         * @param mapping the initial button mapping, or NULL for none
         */
         LogicalButton* AddButton(const std::string& description,
            int buttonSymbol,
            ButtonMapping* mapping = NULL);

         /**
          * Adds a new logical button to this device.  Equivalent to AddButton(description,
          * new ButtonToButton(sourceButton)).
          *
          * @param description a description of the button
          * @param sourceButton the source button
          * @param buttonSymbol : a unique number representing the button to be added
          */
         LogicalButton* AddButton(const std::string& description,
                                  Button* sourceButton,
                                  int buttonSymbol);

         /**
          * Removes a logical button from this device.
          *
          * @param button the button to remove
          */
         void RemoveButton(LogicalButton* button);

         /**
          * Adds a new logical axis.
          *
          * @param description a description of the axis
          * @param mapping the initial axis mapping, or NULL for none
          */
         LogicalAxis* AddAxis(const std::string& description, AxisMapping* mapping = NULL);

         /**
          * Adds a new logical axis.  Equivalent to AddAxis(description,
          * new AxisToAxis(sourceAxis)).
          *
          * @param description a description of the axis
          * @param sourceAxis the source axis
          */
         LogicalAxis* AddAxis(const std::string& description, Axis* sourceAxis);

         /**
          * Removes a logical axis from this device.
          *
          * @param axis the axis to remove
          */
         void RemoveAxis(LogicalAxis* axis);

      protected:

         /**
          * Destructor.
          */
         virtual ~LogicalInputDevice();

      private:

         std::vector<dtCore::RefPtr<ButtonToButton> > mOwnedButtonMappingList;
         std::vector<dtCore::RefPtr<AxisMapping> > mOwnedAxisMappingList;

   };

   /**
    * A logical button.
    */
   class DT_CORE_EXPORT LogicalButton : public Button
   {
      public:

         /**
          * Constructor.
          *
          * @param owner the owner of this button
          * @param description a description of this button
          * @param buttonSymbol : A unique number representing this Button
          * @param mapping the initial button mapping
          */
         LogicalButton(LogicalInputDevice* owner,
                       const std::string& description,
                       int buttonSymbol,
                       ButtonMapping *mapping);

      protected:

         virtual ~LogicalButton() {}

      public:

         /**
          * Sets this button's mapping.
          *
          * @param mapping the new mapping
          */
         void SetMapping(ButtonMapping* mapping);

         /**
          * Returns this button's mapping.
          *
          * @return the current mapping
          */
         ButtonMapping* GetMapping();


      private:

         /**
          * The mapping that determines where this button gets its
          * state.
          */
         ObserverPtr<ButtonMapping> mMapping;
   };


   /**
    * A mapping for a logical button.
    */
   class DT_CORE_EXPORT ButtonMapping : public osg::Referenced
   {
      friend class LogicalButton;

      public:

         ButtonMapping();

      protected:

         virtual ~ButtonMapping();

         /**
          * Sets the target button.
          *
          * @param targetButton the new target button
          */
         virtual void SetTargetButton(LogicalButton* targetButton) = 0;

         /**
          * Gets the target button.
          *
          * @return the current target button
          */
         virtual LogicalButton* GetTargetButton() = 0;
   };


   /**
    * Maps a button to a logical button.
    */
   class DT_CORE_EXPORT ButtonToButton :  public ButtonMapping,
                                          public ButtonHandler,
                                          public ButtonObserver
   {
      public:

         /**
          * Constructor.
          *
          * @param sourceButton the source button
          */
         ButtonToButton(Button* sourceButton);

      protected:

         /**
          * Destructor.
          */
         virtual ~ButtonToButton();

      public:

         /**
          * Sets the source button.
          *
          * @param sourceButton the new source button
          */
         void SetSourceButton(Button* sourceButton);

         /**
          * Returns the source button.
          *
          * @return the current source button
          */
         Button* GetSourceButton();

         /**
          * Sets the target button.
          *
          * @param targetButton the new target button
          */
         virtual void SetTargetButton(LogicalButton* targetButton);

         /**
          * Gets the target button.
          *
          * @return the current target button
          */
         virtual LogicalButton* GetTargetButton();

         /**
          * Called when a button's state has changed.
          *
          * @param button the origin of the event
          * @param oldState the old state of the button
          * @param newState the new state of the button
          */
         virtual bool HandleButtonStateChanged(const Button* button,
                                         bool oldState,
                                         bool newState);

         /// Called when a button's state has changed.
         /// @param button the origin of the event
         /// @param oldState the old state of the button
         /// @param newState the new state of the button
         virtual void OnButtonStateChanged(const Button* button, bool oldState, bool newState);

      private:

         /**
          * The source button.
          */
         ObserverPtr<Button> mSourceButton;

         /**
          * The target button.
          */
         ObserverPtr<LogicalButton> mTargetButton;


         /**
          * Updates the state of the target button.
          */
         void UpdateTargetButtonState();
   };

   /**
   * Maps two buttons to a logical button.
   */
   class DT_CORE_EXPORT ButtonsToButton :  public ButtonMapping,
                                           public ButtonHandler,
                                           public ButtonObserver
   {
   public:

      enum ButtonComboEnum
      {
         BOTH_BUTTONS,  // Must be both buttons to activate.
         SINGLE_BUTTON, // Can be either one or the other, but not both.
         ANY_BUTTONS,   // Any combination of buttons will work.
      };

      /**
      * Constructor.
      *
      * @param firstButton   The first button.
      * @param secondButton  The second button.
      * @param flag          How the buttons can be used.
      */
      ButtonsToButton(Button* firstButton, Button* secondButton, ButtonComboEnum flag = ANY_BUTTONS);

   protected:

      /**
      * Destructor.
      */
      virtual ~ButtonsToButton();

   public:

      /**
      * Sets the first button.
      *
      * @param sourceButton the new source button
      */
      void SetFirstButton(Button* button);

      /**
      * Sets the second button.
      *
      * @param sourceButton the new source button
      */
      void SetSecondButton(Button* button);

      /**
      * Returns the first button.
      *
      * @return the current source button
      */
      Button* GetFirstButton();

      /**
      * Returns the second button.
      *
      * @return the current source button
      */
      Button* GetSecondButton();

      /**
      * Sets the target button.
      *
      * @param targetButton the new target button
      */
      virtual void SetTargetButton(LogicalButton* targetButton);

      /**
      * Gets the target button.
      *
      * @return the current target button
      */
      virtual LogicalButton* GetTargetButton();

      /**
      * Called when a button's state has changed.
      *
      * @param button the origin of the event
      * @param oldState the old state of the button
      * @param newState the new state of the button
      */
      virtual bool HandleButtonStateChanged(const Button* button,
         bool oldState,
         bool newState);

      /// Called when a button's state has changed.
      /// @param button the origin of the event
      /// @param oldState the old state of the button
      /// @param newState the new state of the button
      virtual void OnButtonStateChanged(const Button* button, bool oldState, bool newState);

   private:

      /**
      * The source button.
      */
      ObserverPtr<Button> mFirstButton;
      ObserverPtr<Button> mSecondButton;

      /**
      * The target button.
      */
      ObserverPtr<LogicalButton> mTargetButton;

      ButtonComboEnum  mFlag;


      /**
      * Updates the state of the target button.
      */
      void UpdateTargetButtonState();
   };

   /**
    * A logical axis.
    */
   class DT_CORE_EXPORT LogicalAxis : public Axis
   {
      public:

         /**
          * Constructor.
          *
          * @param owner the owner of this axis
          * @param description a description of this axis
          * @param mapping the initial mapping
          */
         LogicalAxis(LogicalInputDevice* owner,
                     const std::string& description,
                     AxisMapping* mapping);

      protected:

         virtual ~LogicalAxis() {}

      public:

         /**
          * Sets this axis' mapping.
          *
          * @param mapping the new mapping
          */
         void SetMapping(AxisMapping* mapping);

         /**
          * Returns this axis' mapping.
          *
          * @return the current mapping
          */
         AxisMapping* GetMapping();

      private:

         /**
          * The mapping that determines where this axis gets its
          * state.
          */
         ObserverPtr<AxisMapping> mMapping;
   };


   /**
    * A mapping for a logical axis.
    */
   class DT_CORE_EXPORT AxisMapping : public osg::Referenced
   {
      friend class LogicalAxis;

      public:

         AxisMapping();

      protected:

         virtual ~AxisMapping();

         /**
          * Sets the target axis.
          *
          * @param targetAxis the new target axis
          */
         virtual void SetTargetAxis(LogicalAxis* targetAxis) = 0;

         /**
          * Gets the target axis.
          *
          * @return the current target axis
          */
         virtual LogicalAxis* GetTargetAxis() = 0;
   };

   /**
    * Maps an axis to a logical axis with an optional linear transformation.
    */
   class DT_CORE_EXPORT AxisToAxis :   public AxisMapping,
                                       public AxisHandler,
                                       public AxisObserver
   {
      public:

         /**
          * Constructor.
          *
          * @param sourceAxis the source axis
          * @param scale the optional scale parameter
          * @param offset the optional offset parameter
          */
         AxisToAxis(Axis* sourceAxis, double scale = 1.0, double offset = 0.0);

      protected:

         virtual ~AxisToAxis();

      public:

         /**
          * Sets the source axis.
          *
          * @param sourceAxis the new source axis
          */
         void SetSourceAxis(Axis* sourceAxis);

         /**
          * Returns the source axis.
          *
          * @return the current source axis
          */
         Axis* GetSourceAxis();

         /**
          * Sets the target axis.
          *
          * @param targetAxis the new target axis
          */
         virtual void SetTargetAxis(LogicalAxis* targetAxis);

         /**
          * Gets the target axis.
          *
          * @return the current target axis
          */
         virtual LogicalAxis* GetTargetAxis();

         /**
          * Sets this mapping's linear transformation parameters.
          *
          * @param scale the new scale parameter
          * @param offset the new offset parameter
          */
         void SetTransformationParameters(double scale, double offset);

         /**
          * Retrieves this mapping's linear transformation parameters.
          *
          * @param scale a pointer to the location in which to store the scale
          * parameter
          * @param offset a pointer to the location in which to store the offset
          * parameter
          */
         void GetTransformationParameters(double* scale, double* offset) const;

         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          * @return Whether the event was handled.
          */
         virtual bool HandleAxisStateChanged(const Axis* axis,
                                       double oldState,
                                       double newState,
                                       double delta);

         /**
         * Called when an axis' state has changed.
         *
         * @param axis the changed axis
         * @param oldState the old state of the axis
         * @param newState the new state of the axis
         * @param delta a delta value indicating stateless motion
         */
         virtual void OnAxisStateChanged(const Axis* axis, double oldState, double newState, double delta);

      private:
         /// The source axis.
         ObserverPtr<Axis> mSourceAxis;

         /// The target axis.
         ObserverPtr<LogicalAxis> mTargetAxis;

         /// The transformation scale.
         double mScale;

         /// The transformation offset.
         double mOffset;

         /// Updates the state of the target axis.
         bool UpdateTargetAxisState();
   };


   /**
    * Maps any number of source axes to a single target
    * axis.  The value of the target axis will correspond
    * to the value of the last source axis updated.
    */
   class DT_CORE_EXPORT AxesToAxis :   public AxisMapping,
                                       public AxisHandler,
                                       public AxisObserver
   {
      public:

         /**
          * Constructor.
          *
          * @param firstSourceAxis the first source axis, or NULL for none
          * @param secondSourceAxis the second source axis, or NULL for none
          */
         AxesToAxis(Axis* firstSourceAxis = NULL,
                    Axis* secondSourceAxis = NULL);

      protected:

         /**
          * Destructor.
          */
         virtual ~AxesToAxis();

      public:

         /**
          * Adds a source axis.
          *
          * @param sourceAxis the source axis to add
          */
         void AddSourceAxis(Axis* sourceAxis);

         /**
          * Removes a source axis.
          *
          * @param sourceAxis the source axis to remove
          */
         void RemoveSourceAxis(Axis* sourceAxis);

         /**
          * Returns the number of source axes.
          *
          * @return the number of source axes
          */
         int GetNumSourceAxes();

         /**
          * Returns the source axis at the specified index.
          *
          * @param index the index of the axis to retrieve
          * @return the axis at the specified index
          */
         Axis* GetSourceAxis(int index);

         /**
          * Sets the target axis.
          *
          * @param targetAxis the new target axis
          */
         virtual void SetTargetAxis(LogicalAxis* targetAxis);

         /**
          * Gets the target axis.
          *
          * @return the current target axis
          */
         virtual LogicalAxis* GetTargetAxis();

         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual bool HandleAxisStateChanged(const Axis* axis, double oldState, double newState, double delta);

         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual void OnAxisStateChanged(const Axis* axis, double oldState, double newState, double delta);

      private:
         /// The source axes.
         std::vector< ObserverPtr<Axis> > mSourceAxes;

         /// The target axis.
         ObserverPtr<LogicalAxis> mTargetAxis;


         /// Updates the state of the target axis.
         bool UpdateTargetAxisState();
   };


   /**
    * Maps two buttons to a logical axis.
    */
   class DT_CORE_EXPORT ButtonsToAxis :   public AxisMapping,
                                          public ButtonHandler,
                                          public ButtonObserver
   {
      public:

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
         ButtonsToAxis(Button* firstSourceButton,
                       Button* secondSourceButton,
                       double firstButtonValue = -1.0,
                       double secondButtonValue = 1.0,
                       double neutralValue = 0.0);

      protected:

         /**
          * Destructor.
          */
         virtual ~ButtonsToAxis();

      public:

         /**
          * Sets the two source buttons.
          *
          * @param firstSourceButton the first button
          * @param secondSourceButton the second button
          */
         void SetSourceButtons(Button* firstSourceButton,
                               Button* secondSourceButton);

         /**
          * Retrieves pointers to the two source buttons.
          *
          * @param firstSourceButton a pointer to the location in which to store a
          * pointer to the first button
          * @param secondSourceButton a pointer to the location in which to store a
          * pointer to the second button
          */
         void GetSourceButtons(Button** firstSourceButton,
                               Button** secondSourceButton);

         /**
          * Sets the target axis.
          *
          * @param targetAxis the new target axis
          */
         virtual void SetTargetAxis(LogicalAxis* targetAxis);

         /**
          * Gets the target axis.
          *
          * @return the current target axis
          */
         virtual LogicalAxis* GetTargetAxis();

         /**
          * Sets the axis values.
          *
          * @param firstButtonValue the value corresponding to the first button
          * @param secondButtonValue the value corresponding to the second button
          * @param neutralValue the value corresponding to the neutral state
          */
         void SetValues(double firstButtonValue,
                        double secondButtonValue,
                        double neutralValue);

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
         void GetValues(double* firstButtonValue,
                        double* secondButtonValue,
                        double* neutralValue) const;

         /**
          * Called when a button's state has changed.
          *
          * @param button the origin of the event
          * @param oldState the old state of the button
          * @param newState the new state of the button
          */
         virtual bool HandleButtonStateChanged(const Button* button,
                                         bool oldState,
                                         bool newState);

         /// Called when a button's state has changed.
         /// @param button the origin of the event
         /// @param oldState the old state of the button
         /// @param newState the new state of the button
         virtual void OnButtonStateChanged(const Button* button, bool oldState, bool newState);

         // Accessors for child classes
         Button* GetFirstSourceButton() const  { return mFirstSourceButton.get();  }
         Button* GetSecondSourceButton() const { return mSecondSourceButton.get(); }
         LogicalAxis* GetTargetAxis() const    { return mTargetAxis.get();         }
         double GetFirstButtonValue() const    { return mFirstButtonValue;         }
         double GetSecondButtonValue() const   { return mSecondButtonValue;        }
         double GetNeutralValue() const        { return mNeutralValue;             }

      private:

         /**
          * The first source button.
          */
         ObserverPtr<Button> mFirstSourceButton;
         /**
          * The second source button.
          */
         ObserverPtr<Button> mSecondSourceButton;
         /// The target axis.
         ObserverPtr<LogicalAxis> mTargetAxis;
         /// The value corresponding to the first button.
         double mFirstButtonValue;
         /// The value corresponding to the second button.
         double mSecondButtonValue;
         /// The neutral value.
         double mNeutralValue;

         /// Updates the state of the target axis.
         virtual bool UpdateTargetAxisState();
   };


   /**
    * Maps two buttons to a logical axis and passes their values as the delta when
    * they call SetState in UpdateTargetAxisState.
    */
   class DT_CORE_EXPORT DeltaButtonsToAxis : public ButtonsToAxis
   {
      public:

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
         DeltaButtonsToAxis(Button* firstSourceButton,
                       Button* secondSourceButton,
                       double firstButtonValue = -1.0,
                       double secondButtonValue = 1.0,
                       double neutralValue = 0.0);

      protected:

         /**
          * Destructor.
          */
         virtual ~DeltaButtonsToAxis();

      private:

         /// Updates the state of the target axis.
         virtual bool UpdateTargetAxisState();

   };

   /**
    * Maps a button and an axis to an axis, so that the value of the
    * target axis is equal to the value of the source axis when the
    * source button is pressed (the value is zero otherwise).
    */
   class DT_CORE_EXPORT ButtonAxisToAxis :   public AxisMapping,
                                             public ButtonHandler,
                                             public ButtonObserver,
                                             public AxisHandler,
                                             public AxisObserver
   {
      public:

         /**
          * Constructor.
          *
          * @param sourceButton the source button
          * @param sourceAxis the source axis
          */
         ButtonAxisToAxis(Button* sourceButton, Axis* sourceAxis);

      protected:

         /**
          * Destructor.
          */
         virtual ~ButtonAxisToAxis();

      public:

         /**
          * Sets the source button.
          *
          * @param sourceButton the new source button
          */
         void SetSourceButton(Button* sourceButton);

         /**
          * Returns the source button.
          *
          * @return the current source button
          */
         Button* GetSourceButton();

         /**
          * Sets the source axis.
          *
          * @param sourceAxis the new source axis
          */
         void SetSourceAxis(Axis* sourceAxis);

         /**
          * Returns the source axis.
          *
          * @return the current source axis.
          */
         Axis* GetSourceAxis();

         /**
          * Sets the target axis.
          *
          * @param targetAxis the new target axis
          */
         virtual void SetTargetAxis(LogicalAxis* targetAxis);

         /**
          * Gets the target axis.
          *
          * @return the current target axis
          */
         virtual LogicalAxis* GetTargetAxis();

         /**
          * Called when a button's state has changed.
          *
          * @param button the origin of the event
          * @param oldState the old state of the button
          * @param newState the new state of the button
          */
         virtual bool HandleButtonStateChanged(const Button* button, bool oldState, bool newState);

         /// Called when a button's state has changed.
         /// @param button the origin of the event
         /// @param oldState the old state of the button
         /// @param newState the new state of the button
         virtual void OnButtonStateChanged(const Button* button, bool oldState, bool newState);

         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual bool HandleAxisStateChanged(const Axis* axis, double oldState, double newState, double delta);

         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual void OnAxisStateChanged(const Axis* axis, double oldState, double newState, double delta);

      private:
         /// The source button.
         ObserverPtr<Button> mSourceButton;

         /// The source axis.
         ObserverPtr<Axis> mSourceAxis;

         /// The target axis.
         ObserverPtr<LogicalAxis> mTargetAxis;

         /// Updates the state of the target axis.
         bool UpdateTargetAxisState();
   };
}


#endif // DELTA_LOGICALINPUTDEVICE
