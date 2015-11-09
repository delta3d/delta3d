#include <dtABC/proximitytrigger.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

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

   // Set the update callback which keeps track of traversal numbers.
   GetOSGNode()->setUpdateCallback(new NodeCallback(this));
}
