#ifndef DELTA_INPUTDEVICE
#define DELTA_INPUTDEVICE

// inputdevice.h: Declaration of the InputDevice class.
//
//////////////////////////////////////////////////////////////////////


#include <set>
#include <string>
#include <vector>

#include <osg/ref_ptr>

#include "dtCore/base.h"


namespace dtCore
{
   class InputDeviceFeature;
   class Button;
   class ButtonListener;
   class Axis;
   class AxisListener;


   /**
    * Represents an input device.
    */
   class DT_EXPORT InputDevice : public Base
   {
      friend class Button;
      friend class Axis;
      
      
      DECLARE_MANAGEMENT_LAYER(InputDevice)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         InputDevice(std::string name = "InputDevice");

         /**
          * Destructor.
          */
         virtual ~InputDevice();

         /**
          * Returns the number of features (buttons, axes, etc.) contained in 
          * this device.
          *
          * @return the number of features contained in this device
          */
         int GetFeatureCount() const;

         /**
          * Returns a pointer to the specified feature.
          *
          * @param index the index of the desired feature
          * @return a pointer to the feature
          */
         InputDeviceFeature* GetFeature(int index);

         /**
          * Returns the number of features (buttons, axes, etc.) contained in 
          * this device.
          *
          * @return the number of features contained in this device
          */
         int GetButtonCount() const;

         /**
          * Returns a pointer to the specified feature.
          *
          * @param index the index of the desired feature
          * @return a pointer to the feature
          */
         Button* GetButton(int index);

         /**
          * Returns the number of features (buttons, axes, etc.) contained in 
          * this device.
          *
          * @return the number of features contained in this device
          */
         int GetAxisCount() const;

         /**
          * Returns a pointer to the specified feature.
          *
          * @param index the index of the desired feature
          * @return a pointer to the feature
          */
         Axis* GetAxis(int index);

         /**
          * Adds a button listener.
          *
          * @param buttonListener a pointer to the listener to add
          */
         void AddButtonListener(ButtonListener* buttonListener);

         /**
          * Removes a button listener.
          *
          * @param buttonListener a pointer to the listener to remove
          */
         void RemoveButtonListener(ButtonListener* buttonListener);
         
         /**
          * Adds an axis listener.
          *
          * @param axisListener a pointer to the listener to add
          */
         void AddAxisListener(AxisListener* axisListener);

         /**
          * Removes an axis listener.
          *
          * @param axisListener a pointer to the listener to remove
          */
         void RemoveAxisListener(AxisListener* axisListener);
         

      protected:

         /**
          * Adds a feature to this device.
          *
          * @param feature a pointer to the feature to add
          */
         void AddFeature(InputDeviceFeature* feature);

         /**
          * Removes a feature from this device.
          *
          * @param feature a pointer to the feature to remove
          */
         void RemoveFeature(InputDeviceFeature* feature);


      private:
         
         /**
          * The list of features.
          */
         std::vector< osg::ref_ptr<InputDeviceFeature> > mFeatures;

         /**
          * The list of buttons.
          */
         std::vector< osg::ref_ptr<Button> > mButtons;

         /**
          * The list of axes.
          */
         std::vector< osg::ref_ptr<Axis> > mAxes;
         
         /**
          * The set of button listeners.
          */
         std::set<ButtonListener*> mButtonListeners;
         
         /**
          * The set of axis listeners.
          */
         std::set<AxisListener*> mAxisListeners;
   };


   /**
    * The abstract base class of all input device features: buttons, axes,
    * etc.
    */
   class DT_EXPORT InputDeviceFeature : public osg::Referenced
   {
      public:

         /**
          * Constructor.
          *
          * @param owner the owner of this feature
          * @param description a description of this feature
          */
         InputDeviceFeature(InputDevice* owner, std::string description);

         /**
          * Destructor.
          */
         virtual ~InputDeviceFeature() = 0;

         /**
          * Returns a pointer to the owner of this feature.
          *
          * @return a pointer to the owner of this feature
          */
         InputDevice* GetOwner() const;

         /**
          * Sets the description of this feature.
          *
          * @param description the new description
          */
         void SetDescription(std::string description);

         /**
          * Returns a description of this feature.
          *
          * @return a description of this feature
          */
         std::string GetDescription() const;


      private:

         /**
          * The owner of this feature.
          */
         InputDevice* mOwner;

         /**
          * A description of this feature.
          */
         std::string mDescription;
   };


   /**
    * Buttons are features with binary state.
    */
   class DT_EXPORT Button : public InputDeviceFeature
   {
      public:
      
         /**
          * Constructor.
          *
          * @param owner the owner of this button
          * @param description a description of this button
          */
         Button(InputDevice* owner, std::string description);

         /**
          * Sets the state of this button.
          *
          * @param state the new state
          */
         void SetState(bool state);

         /**
          * Returns the current state of this button.
          *
          * @return true if this button is pressed, false
          * otherwise
          */
         bool GetState() const;

         /**
          * Adds a button listener.
          *
          * @param buttonListener a pointer to the listener to add
          */
         void AddButtonListener(ButtonListener* buttonListener);

         /**
          * Removes a button listener.
          *
          * @param buttonListener a pointer to the listener to remove
          */
         void RemoveButtonListener(ButtonListener* buttonListener);


      private:

         /**
          * The state of this button.
          */
         bool mState;

         /**
          * Listeners to this button.
          */
         std::set<ButtonListener*> mButtonListeners; 
   };


   /**
    * An interface for objects interested in button state changes.
    */
   class DT_EXPORT ButtonListener
   {
      public:

         /**
          * Called when a button's state has changed.
          *
          * @param button the origin of the event
          * @param oldState the old state of the button
          * @param newState the new state of the button
          */
         virtual void ButtonStateChanged(Button* button,
                                         bool oldState,
                                         bool newState) {}
   };


   /**
    * Axes are features with double-valued state.
    */
   class DT_EXPORT Axis : public InputDeviceFeature
   {
      public:

         /**
          * Constructor.
          *
          * @param owner the owner of this axis
          * @param description a description of this axis
          */
         Axis(InputDevice* owner, std::string description);

         /**
          * Sets the state of this axis.  The delta value is used to
          * indicate stateless motion, such as mouse scrolling.
          *
          * @param state the new state
          * @param delta the optional delta value
          */
         void SetState(double state, double delta = 0.0);

         /**
          * Returns the current state of this axis.
          *
          * @return the current state of this axis
          */
         double GetState() const;

         /**
          * Adds an axis listener.
          *
          * @param axisListener a pointer to the listener to add
          */
         void AddAxisListener(AxisListener* axisListener);

         /**
          * Removes an axis listener.
          *
          * @param axisListener a pointer to the listener to remove
          */
         void RemoveAxisListener(AxisListener* axisListener);


      private:

         /**
          * The state of this axis.
          */
         double mState;

         /**
          * Listeners to this axis.
          */
         std::set<AxisListener*> mAxisListeners;
   };


   /**
    * An interface for objects interested in changes to axes.
    */
   class DT_EXPORT AxisListener
   {
      public:
      
         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual void AxisStateChanged(Axis* axis,
                                       double oldState, 
                                       double newState, 
                                       double delta) {}
   };
};


#endif // DELTA_INPUTDEVICE
