#include <dtAudio/listener.h>
#include <dtCore/scene.h>

// namespaces
using namespace   dtAudio;
using namespace   dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Listener)

/********************************
 ** Protected Member Functions **
 ********************************
 */
/**
 * Constructor, user does not create directly
 * instead requests the listener from AudioManager
 */
Listener::Listener()
{
   SetName("Listener");
   RegisterInstance(this);

   // Default collision category = 13
   SetCollisionCategoryBits( UNSIGNED_BIT(13) );
}



/**
 * Destructor, user does not delete directly
 * AudioManager handles destruction
 */
Listener::~Listener()
{
    DeregisterInstance(this);
}
