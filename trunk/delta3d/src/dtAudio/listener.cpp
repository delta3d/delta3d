#include <dtAudio/listener.h>



// namespaces
using namespace   dtAudio;
using namespace   dtCore;



IMPLEMENT_MANAGEMENT_LAYER(Listener)



/********************************
/** Protected Member Functions **
/********************************
/**
 * Constructor, user does not create directly
 * instead requests the listener from AudioManager
 */
Listener::Listener()
:  Transformable()
{
    RegisterInstance(this);
}



/**
 * Destructor, user does not delete directly
 * AudioManager handles destruction
 */
Listener::~Listener()
{
    DeregisterInstance(this);
}
