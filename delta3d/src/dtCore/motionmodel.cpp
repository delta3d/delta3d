// motionmodel.cpp: Implementation of the MotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/motionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(MotionModel)

/**
 * Constructor.
 *
 * @param name the instance name
 */
MotionModel::MotionModel(const std::string& name)
   : Base(name),
     mEnabled(true)
{
   RegisterInstance(this);
}

/**
 * Destructor.
 */
MotionModel::~MotionModel()
{
   DeregisterInstance(this);
}

/**
 * Sets the target of this motion model.
 *
 * @param target the new target
 */
void MotionModel::SetTarget(Transformable* target)
{
   mTarget = target;
}

/**
 * Returns the target of this motion model.
 *
 * @return the current target
 */
Transformable* MotionModel::GetTarget()
{
   return mTarget.get();
}

/**
 * Enables or disables this motion model.
 *
 * @param enabled true to enable this motion model, false
 * to disable it
 */
void MotionModel::SetEnabled(bool enabled)
{
   mEnabled = enabled;
}

/**
 * Checks whether or not this motion model is enabled.
 *
 * @return true if this motion model is enabled, false
 * if it is disabled
 */
bool MotionModel::IsEnabled() const
{
   return mEnabled;
}

}
