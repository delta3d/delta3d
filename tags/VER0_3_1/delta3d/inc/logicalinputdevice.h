#ifndef DELTA_LOGICALINPUTDEVICE
#define DELTA_LOGICALINPUTDEVICE

// logicalinputdevice.h: Declaration of the LogicalInputDevice class.
//
//////////////////////////////////////////////////////////////////////


#include "inputdevice.h"


namespace dtCore
{
   class LogicalButton;
   class ButtonMapping;
   class LogicalAxis;
   class AxisMapping;


   /**
    * A logical input device.
    */
   class DT_EXPORT LogicalInputDevice : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(LogicalInputDevice)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         LogicalInputDevice(std::string name = "LogicalInputDevice");

         /**
          * Destructor.
          */
         virtual ~LogicalInputDevice();

         /**
          * Adds a new logical button to this device.
          *
          * @param description a description of the button
          * @param mapping the initial button mapping, or NULL for none
          */
         LogicalButton* AddButton(std::string description, ButtonMapping* mapping = NULL);

         /**
          * Adds a new logical button to this device.  Equivalent to AddButton(description,
          * new ButtonToButton(sourceButton)).
          *
          * @param description a description of the button
          * @param sourceButton the source button
          */
         LogicalButton* AddButton(std::string description, Button* sourceButton);
         
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
         LogicalAxis* AddAxis(std::string description, AxisMapping* mapping = NULL);

         /**
          * Adds a new logical axis.  Equivalent to AddAxis(description,
          * new AxisToAxis(sourceAxis)).
          *
          * @param description a description of the axis
          * @param sourceAxis the source axis
          */
         LogicalAxis* AddAxis(std::string description, Axis* sourceAxis);
         
         /**
          * Removes a logical axis from this device.
          *
          * @param axis the axis to remove
          */
         void RemoveAxis(LogicalAxis* axis);
   };

   
   /**
    * A logical button.
    */
   class DT_EXPORT LogicalButton : public Button
   {
      public:

         /**
          * Constructor.
          *
          * @param owner the owner of this button
          * @param description a description of this button
          * @param mapping the initial button mapping
          */
         LogicalButton(LogicalInputDevice* owner,
                       std::string description, 
                       ButtonMapping *mapping);

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
         osg::ref_ptr<ButtonMapping> mMapping;
   };
   

   /**
    * A mapping for a logical button.
    */
   class DT_EXPORT ButtonMapping : public osg::Referenced
   {
      friend class LogicalButton;

      public:

         /**
          * Constructor.
          */
         ButtonMapping();

         /**
          * Destructor.
          */
         virtual ~ButtonMapping();


      protected:

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
   class DT_EXPORT ButtonToButton : public ButtonMapping,
                                    public ButtonListener
   {
      public:

         /**
          * Constructor.
          *
          * @param sourceButton the source button
          */
         ButtonToButton(Button* sourceButton);

         /**
          * Destructor.
          */
         virtual ~ButtonToButton();

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
         virtual void ButtonStateChanged(Button* button,
                                         bool oldState,
                                         bool newState);

      private:

         /**
          * The source button.
          */
         osg::ref_ptr<Button> mSourceButton;

         /**
          * The target button.
          */
         osg::ref_ptr<LogicalButton> mTargetButton;


         /**
          * Updates the state of the target button.
          */
         void UpdateTargetButtonState();
   };


   /**
    * A logical axis.
    */
   class DT_EXPORT LogicalAxis : public Axis
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
                     std::string description,
                     AxisMapping* mapping);

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
         osg::ref_ptr<AxisMapping> mMapping;
   };


   /**
    * A mapping for a logical axis.
    */
   class DT_EXPORT AxisMapping : public osg::Referenced
   {
      friend class LogicalAxis;

      public:

         /**
          * Constructor.
          */
         AxisMapping();

         /**
          * Destructor.
          */
         virtual ~AxisMapping();


      protected:

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
   class DT_EXPORT AxisToAxis : public AxisMapping,
                                public AxisListener
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

         /**
          * Destructor.
          */
         virtual ~AxisToAxis();

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
          */
         virtual void AxisStateChanged(Axis* axis,
                                       double oldState, 
                                       double newState, 
                                       double delta);

      private:

         /**
          * The source axis.
          */
         osg::ref_ptr<Axis> mSourceAxis;

         /**
          * The target axis.
          */
         osg::ref_ptr<LogicalAxis> mTargetAxis;

         /**
          * The transformation scale.
          */
         double mScale;

         /**
          * The transformation offset.
          */
         double mOffset;


         /**
          * Updates the state of the target axis.
          */
         void UpdateTargetAxisState();
   };


   /**
    * Maps any number of source axes to a single target
    * axis.  The value of the target axis will correspond
    * to the value of the last source axis updated.
    */
   class DT_EXPORT AxesToAxis : public AxisMapping,
                                public AxisListener
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

         /**
          * Destructor.
          */
         virtual ~AxesToAxis();
         
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
         virtual void AxisStateChanged(Axis* axis,
                                       double oldState, 
                                       double newState, 
                                       double delta);
      
         
      private:
      
         /**
          * The source axes.
          */
         std::vector< osg::ref_ptr<Axis> > mSourceAxes;
         
         /**
          * The target axis.
          */
         osg::ref_ptr<LogicalAxis> mTargetAxis;
         
         
         /**
          * Updates the state of the target axis.
          */
         void UpdateTargetAxisState();
   };
   
   
   /**
    * Maps two buttons to a logical axis.
    */
   class DT_EXPORT ButtonsToAxis : public AxisMapping,
                                   public ButtonListener
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

         /**
          * Destructor.
          */
         virtual ~ButtonsToAxis();

         /**
          * Sets the two source buttons.
          *
          * @param firstButton the first button
          * @param secondButton the second button
          */
         void SetSourceButtons(Button* firstSourceButton,
                               Button* secondSourceButton);

         /**
          * Retrieves pointers to the two source buttons.
          *
          * @param firstButton a pointer to the location in which to store a
          * pointer to the first button
          * @param secondButton a pointer to the location in which to store a
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
         virtual void ButtonStateChanged(Button* button,
                                         bool oldState,
                                         bool newState);


      private:

         /**
          * The first source button.
          */
         osg::ref_ptr<Button> mFirstSourceButton;

         /**
          * The second source button.
          */
         osg::ref_ptr<Button> mSecondSourceButton;

         /**
          * The target axis.
          */
         osg::ref_ptr<LogicalAxis> mTargetAxis;

         /**
          * The value corresponding to the first button.
          */
         double mFirstButtonValue;

         /**
          * The value corresponding to the second button.
          */
         double mSecondButtonValue;

         /**
          * The neutral value.
          */
         double mNeutralValue;


         /**
          * Updates the state of the target axis.
          */
         void UpdateTargetAxisState();
   };
   
   
   /**
    * Maps a button and an axis to an axis, so that the value of the
    * target axis is equal to the value of the source axis when the
    * source button is pressed (the value is zero otherwise).
    */
   class DT_EXPORT ButtonAxisToAxis : public AxisMapping,
                                      public ButtonListener,
                                      public AxisListener
   {
      public:
      
         /**
          * Constructor.
          *
          * @param sourceButton the source button
          * @param sourceAxis the source axis
          */
         ButtonAxisToAxis(Button* sourceButton, Axis* sourceAxis);
         
         /**
          * Destructor.
          */
         virtual ~ButtonAxisToAxis();
         
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
         virtual void ButtonStateChanged(Button* button,
                                         bool oldState,
                                         bool newState);
         
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
                                       double delta);
         
         
      private:
      
         /**
          * The source button.
          */
         osg::ref_ptr<Button> mSourceButton;
         
         /**
          * The source axis.
          */
         osg::ref_ptr<Axis> mSourceAxis;
         
         /**
          * The target axis.
          */
         osg::ref_ptr<LogicalAxis> mTargetAxis;
         
         
         /**
          * Updates the state of the target axis.
          */
         void UpdateTargetAxisState();
   };
};


#endif // DELTA_LOGICALINPUTDEVICE
