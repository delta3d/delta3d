#include <dtActors/labelactorproxy.h>
#include <dtABC/labelactor.h>

namespace dtActors
{


   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString LabelActorProxy::CLASS_NAME("dtActors::LabelActor");

   /////////////////////////////////////////////////////////////////////////////
   LabelActorProxy::LabelActorProxy()
      : BaseClass()
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   LabelActorProxy::~LabelActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtABC::LabelActor);
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      dtABC::LabelActor* actor = NULL;
      GetDrawable(actor);

      // Get the properties for the actor.
      dtABC::LabelActor::ActorPropertyArray properties;
      actor->CreateActorProperties(properties);

      // Add the properties to the proxy.
      dtABC::LabelActor::ActorPropertyArray::iterator curProperty = properties.begin();
      dtABC::LabelActor::ActorPropertyArray::iterator endPropertyArray = properties.end();
      for (; curProperty != endPropertyArray; ++curProperty)
      {
         AddProperty(curProperty->get());
      }
   }
}
