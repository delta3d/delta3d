#include <listener.h>



// namespaces
using namespace   dtAudio;
using namespace   dtCore;



IMPLEMENT_MANAGEMENT_LAYER(Listener)



Listener::Listener()
:  Transformable()
{
    RegisterInstance(this);
}



Listener::~Listener()
{
    DeregisterInstance(this);
}
