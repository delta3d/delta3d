#include <dtABC/proximitytrigger.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/collisioncategorydefaults.h>

#include <assert.h>

using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER(ProximityTrigger)

////////////////////////////////////////////////////////////////////////////////
ProximityTrigger::ProximityTrigger(const std::string& name)
   : Transformable(name)
   , mTrigger(new Trigger("InternalProximityTrigger"))
   , mLastTraversalNumber(0)
{
   RegisterInstance(this);

   // Enable the internal trigger.
   mTrigger->SetEnabled(true);

   // Give a default collision shape and size.
   SetCollisionSphere(5.0f);

   // By default, we want the trigger itself to collide with anything
   SetCollisionCollideBits(COLLISION_CATEGORY_MASK_ALL);
   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER);

   // Set the update callback which keeps track of traversal numbers.
   GetOSGNode()->setUpdateCallback(new NodeCallback(this));
}

////////////////////////////////////////////////////////////////////////////////
bool ProximityTrigger::FilterContact(dContact* contact, Transformable* collider)
{
   // If the difference between our last known traversal number and the
   // traversal number set on the last collision with this transformable
   // is greater than 1, fire the internal trigger.
   //
   // In other words, only fire the trigger if this particular collider is
   // entering the ProximityTrigger.
   if (mLastTraversalNumber - mTraversalNumberMap[collider] > 1)
   {
      mTrigger->Fire();
   }

   mTraversalNumberMap[collider] = mLastTraversalNumber;

   // Return false because we know this is not a physical and we do not want physics applied
   // in Scene. The dynamic_cast in there would filter it out anyways, but still...
   return false;
}
