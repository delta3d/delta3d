// motionmodel.h: Declaration of the MotionModel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_MOTION_MODEL
#define DELTA_MOTION_MODEL

#include <osg/ref_ptr>

#include "base.h"
#include "transformable.h"

namespace dtCore
{
   /**
    * The base class for all motion models: objects that update the
    * transforms of Transformable objects according to input gathered
    * from physical or logical input devices.
    */
   class DT_EXPORT MotionModel : public Base
   {
      DECLARE_MANAGEMENT_LAYER(MotionModel)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         MotionModel(std::string name = "MotionModel");

         /**
          * Destructor.
          */
         virtual ~MotionModel();
         
         /**
          * Sets the target of this motion model.
          *
          * @param target the new target
          */
         void SetTarget(Transformable* target);
         
         /**
          * Returns the target of this motion model.
          *
          * @return the current target
          */
         Transformable* GetTarget();
         
         /**
          * Enables or disables this motion model.
          *
          * @param enabled true to enable this motion model, false
          * to disable it
          */
         void SetEnabled(bool enabled);
         
         /**
          * Checks whether or not this motion model is enabled.
          *
          * @return true if this motion model is enabled, false
          * if it is disabled
          */
         bool IsEnabled() const;
         
         
      private:
      
         /**
          * The motion model target.
          */
         osg::ref_ptr<Transformable> mTarget;
         
         /**
          * Whether or not the motion model is enabled.
          */
         bool mEnabled;
   };
};

#endif // DELTA_MOTION_MODEL