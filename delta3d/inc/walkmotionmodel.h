#ifndef DELTA_WALKMOTIONMODEL
#define DELTA_WALKMOTIONMODEL

// walkmotionmodel.h: Declaration of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////


#include "keyboard.h"
#include "mouse.h"
#include "logicalinputdevice.h"
#include "motionmodel.h"
#include "inputdevice.h"
#include "scene.h"
#include "system.h"

namespace dtCore
{
   /**
    * A motion model that simulates the action of walking or driving.
    */
   class DT_EXPORT WalkMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(WalkMotionModel)


      public:

         /**
          * Constructor.
          *
          * @param keyboard the keyboard instance, or NULL to
          * avoid creating default input mappings
          * @param mouse the mouse instance, or NULL to avoid
          * creating default input mappings
          */
         WalkMotionModel(Keyboard* keyboard = NULL,
                         Mouse* mouse = NULL);

         /**
          * Destructor.
          */
         virtual ~WalkMotionModel();
         
         /**
          * Sets the active Scene, which is used for ground following.
          *
          * @param scene the active scene
          */
         void SetScene(Scene* scene);
         
         /**
          * Returns the active Scene.
          *
          * @return the active Scene
          */
         Scene* GetScene();
         
         /**
          * Sets the input axes to a set of default mappings for mouse
          * and keyboard.
          *
          * @param keyboard the keyboard instance
          * @param mouse the mouse instance
          */
         void SetDefaultMappings(Keyboard* keyboard, Mouse* mouse);
         
         /**
          * Sets the axis that moves the target forwards (for positive
          * values) or backwards (for negative values).
          *
          * @param walkForwardBackwardAxis the new forward/backward axis
          */
         void SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis);
         
         /**
          * Returns the axis that moves the target forwards (for positive
          * values) or backwards (for negative values).
          *
          * @return the current forward/backward axis
          */
         Axis* GetWalkForwardBackwardAxis();
         
         /**
          * Sets the axis that turns the target left (for negative values)
          * or right (for positive values).
          *
          * @param turnLeftRightAxis the new turn left/right axis
          */
         void SetTurnLeftRightAxis(Axis* turnLeftRightAxis);
         
         /**
          * Returns the axis that turns the target left (for negative values)
          * or right (for positive values).
          *
          * @return the current turn left/right axis
          */
         Axis* GetTurnLeftRightAxis();
         
         /**
          * Sets the axis that sidesteps the target left (for negative values)
          * or right (for positive values).
          *
          * @param sidestepLeftRightAxis the new sidestep left/right axis
          */
         void SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis);
         
         /**
          * Returns the axis that sidesteps the target left (for negative values)
          * or right (for positive values).
          *
          * @return the current sidestep left/right axis
          */
         Axis* GetSidestepLeftRightAxis();
         
         /**
          * Sets the maximum walk speed (meters per second).
          *
          * @param maximumWalkSpeed the new maximum walk speed
          */
         void SetMaximumWalkSpeed(float maximumWalkSpeed);
         
         /**
          * Returns the maximum walk speed (meters per second).
          *
          * @return the current maximum walk speed
          */
         float GetMaximumWalkSpeed();
         
         /**
          * Sets the maximum turn speed (degrees per second).
          *
          * @param maximumTurnSpeed the new maximum turn speed
          */
         void SetMaximumTurnSpeed(float maximumTurnSpeed);
         
         /**
          * Returns the maximum turn speed (degrees per second).
          *
          * @return the current maximum turn speed
          */
         float GetMaximumTurnSpeed();
         
         /**
          * Sets the maximum sidestep speed (meters per second).
          *
          * @param maximumSidestepSpeed the new maximum sidestep speed
          */
         void SetMaximumSidestepSpeed(float maximumSidestepSpeed);
         
         /**
          * Returns the maximum sidestep speed (meters per second).
          *
          * @return the current maximum sidestep speed
          */
         float GetMaximumSidestepSpeed();
         
         /**
          * Sets the height to maintain above the terrain (meters).
          *
          * @param heightAboveTerrain the height to maintain above the
          * terrain
          */
         void SetHeightAboveTerrain(float heightAboveTerrain);
         
         /**
          * Returns the height to maintain above the terrain (meters).
          *
          * @return the height to maintain above the terrain
          */
         float GetHeightAboveTerrain();
         
         /**
          * Message handler callback.
          *
          * @param data the message data
          */
         virtual void OnMessage(MessageData *data);
         
         
      private:
         
         /**
          * A reference to the Scene, used for ground following.
          */
         osg::ref_ptr<Scene> mScene;
         
         /**
          * The default input device.
          */
         osg::ref_ptr<LogicalInputDevice> mDefaultInputDevice;
         
         /**
          * The left button up/down mapping.
          */
         ButtonAxisToAxis* mLeftButtonUpDownMapping;
         
         /**
          * The left button right/left mapping.
          */
         ButtonAxisToAxis* mLeftButtonLeftRightMapping;
         
         /**
          * The right button left/right mapping.
          */
         ButtonAxisToAxis* mRightButtonLeftRightMapping;
         
         /**
          * The arrow key up/down mapping.
          */
         ButtonsToAxis* mArrowKeysUpDownMapping;
         
         /**
          * The arrow key left/right mapping.
          */
         ButtonsToAxis* mArrowKeysLeftRightMapping;
         
         /**
          * The a/d key left/right mapping.
          */
         ButtonsToAxis* mADKeysLeftRightMapping;
         
         /**
          * The default walk forward/backward axis.
          */
         LogicalAxis* mDefaultWalkForwardBackwardAxis;
         
         /**
          * The default turn left/right axis.
          */
         LogicalAxis* mDefaultTurnLeftRightAxis;
         
         /**
          * The default sidestep left/right axis.
          */
         LogicalAxis* mDefaultSidestepLeftRightAxis;
         
         /**
          * The axis that moves the target forwards or backwards.
          */
         Axis* mWalkForwardBackwardAxis;
         
         /**
          * The axis that turns the target to the left or right.
          */
         Axis* mTurnLeftRightAxis;
         
         /**
          * The axis that sidesteps the target left or right.
          */
         Axis* mSidestepLeftRightAxis;
         
         /**
          * The maximum walk speed (meters per second).
          */
         float mMaximumWalkSpeed;
         
         /**
          * The maximum turn speed (degrees per second).
          */
         float mMaximumTurnSpeed;
         
         /**
          * The maximum sidestep speed (meters per second).
          */
         float mMaximumSidestepSpeed;
         
         /**
          * The height to maintain above terrain (meters).
          */
         float mHeightAboveTerrain;
         
         /**
          * The current downward speed.
          */
         float mDownwardSpeed;
   };
};


#endif // DELTA_WALKMOTIONMODEL
