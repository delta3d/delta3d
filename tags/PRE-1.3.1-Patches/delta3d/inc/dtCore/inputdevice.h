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

#ifndef DELTA_INPUTDEVICE
#define DELTA_INPUTDEVICE

// inputdevice.h: Declaration of the InputDevice class.
//
//////////////////////////////////////////////////////////////////////


#include <list>
#include <string>
#include <vector>

#include <dtCore/base.h>
#include <dtCore/refptr.h>

namespace dtCore
{
   class InputDeviceFeature;
   class Button;
   class ButtonListener;
   class Axis;
   class AxisListener;

   /// Represents an input device.
   class DT_CORE_EXPORT InputDevice : public Base
   {
      friend class Button;
      friend class Axis;      

      DECLARE_MANAGEMENT_LAYER(InputDevice)

   public:
      typedef std::vector< dtCore::RefPtr<InputDeviceFeature> > FeatureVector;
      typedef std::vector< dtCore::RefPtr<Button> > ButtonVector;
      typedef std::vector< dtCore::RefPtr<Axis> > AxisVector;

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         InputDevice(const std::string& name = "InputDevice");

      protected:

         /**
          * Destructor.
          */
         virtual ~InputDevice();

      public:

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
         const Button* GetButton(int index) const;

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
         const Axis* GetAxis(int index) const;

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

         /// Adds a feature to this device.
         /// @param feature a pointer to the feature to add
         void AddFeature(InputDeviceFeature* feature);

         /// Removes a feature from this device.
         /// @param feature a pointer to the feature to remove
         void RemoveFeature(InputDeviceFeature* feature);

         const ButtonVector& GetButtons() const { return mButtons; }
         ButtonVector& GetButtons() { return mButtons; }

         const AxisVector& GetAxes() const { return mAxes; }
         AxisVector& GetAxes() { return mAxes; }

      private:
         FeatureVector mFeatures;  ///< The list of features.
         ButtonVector mButtons;  ///< The list of buttons.
         AxisVector mAxes;  ///< The list of axes.

         typedef std::list<ButtonListener*> ButtonListenerList; ///< A container of ButtonListeners.
         typedef std::list<AxisListener*> AxisListenerList;     ///< A container of AxisListeners.

         ButtonListenerList mButtonListeners;  ///< The container of ButtonListeners.
         AxisListenerList mAxisListeners;      ///< The container of AxisListeners.
   };


   /// The base class of all input device features: buttons, axes, etc.
   class DT_CORE_EXPORT InputDeviceFeature : public osg::Referenced
   {
      public:

         /**
          * Constructor.
          *
          * @param owner the owner of this feature
          * @param description a description of this feature
          */
         InputDeviceFeature(InputDevice* owner, const std::string& description);

      protected:

         /**
          * Destructor.
          */
         virtual ~InputDeviceFeature() = 0;

      public:

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
         void SetDescription(const std::string& description);

         /**
          * Returns a description of this feature.
          *
          * @return a description of this feature
          */
         std::string GetDescription() const;

      private:

         /// The owner of this feature.
         InputDevice* mOwner;

         /// A description of this feature.
         std::string mDescription;
   };


   /// Buttons are features with binary state.
   class DT_CORE_EXPORT Button : public InputDeviceFeature
   {
   public:
      typedef std::list<ButtonListener*> ButtonListenerList;  ///< A container of ButtonListeners
      
         /**
          * Constructor.
          *
          * @param owner the owner of this button
          * @param description a description of this button
          */
         Button(InputDevice* owner, const std::string& description);

      protected:

         virtual ~Button() {}

      public:
         /// Sets the state of this button.
         /// @param state the new state
         /// @return The result of the listeners
         bool SetState(bool state);

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

         /// Inserts the listener into the list at a position BEFORE pos.
         void InsertButtonListener(const ButtonListenerList::value_type& pos, ButtonListener* bl);

         const ButtonListenerList& GetListeners() const { return mButtonListeners; }

      private:
         bool mState;  ///< The state of this button.
         ButtonListenerList mButtonListeners;  ///< Listeners to this button.
   };


   /// An interface for objects interested in button state changes.
   class DT_CORE_EXPORT ButtonListener
   {
   public:

      virtual ~ButtonListener() {}

      /// Called when a button's state has changed.
      /// @param button the origin of the event
      /// @param oldState the old state of the button
      /// @param newState the new state of the button
      virtual bool ButtonStateChanged(const Button* button, bool oldState, bool newState)=0;
   };

   /// Axes are features with double-valued state.
   class DT_CORE_EXPORT Axis : public InputDeviceFeature
   {
      public:
         typedef std::list<AxisListener*> AxisListenerList;

         /**
          * Constructor.
          *
          * @param owner the owner of this axis
          * @param description a description of this axis
          */
         Axis(InputDevice* owner, const std::string& description);

      protected:

         virtual ~Axis() {}

      public:

         /**
          * Sets the state of this axis.  The delta value is used to
          * indicate stateless motion, such as mouse scrolling.
          *
          * @param state the new state
          * @param delta the optional delta value
          */
         bool SetState(double state, double delta = 0.0);

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

         const AxisListenerList& GetListeners() const { return mAxisListeners; }

         /// Inserts the listener into the list at a position BEFORE pos.
         void InsertAxisListener(const AxisListenerList::value_type& pos, AxisListener* al);

      private:
         double mState;  ///< The state of this axis.
         AxisListenerList mAxisListeners;  ///< Listeners to this axis.
   };

   /// An interface for objects interested in changes to axes.
   class DT_CORE_EXPORT AxisListener
   {
   public:

      virtual ~AxisListener() {}

      /**
      * Called when an axis' state has changed.
      *
      * @param axis the changed axis
      * @param oldState the old state of the axis
      * @param newState the new state of the axis
      * @param delta a delta value indicating stateless motion
      */
      virtual bool AxisStateChanged(const Axis* axis, double oldState, double newState, double delta)=0;
   };
};


#endif // DELTA_INPUTDEVICE
