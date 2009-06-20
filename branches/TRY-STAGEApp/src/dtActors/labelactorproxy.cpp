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
   void LabelActorProxy::CreateActor()
   {
      SetActor(*new dtABC::LabelActor);
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      dtABC::LabelActor* actor = NULL;
      GetActor(actor);

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

      // REMOVE USELESS PROPERTIES - These properties really should not show in
      // STAGE and ought to be completely removed from the object.
      // However, the overhead is part of sub-classing GameActor.
      RemoveProperty("Show Collision Geometry"); //"ODE Show Collision Geometry"
      RemoveProperty(TransformableActorProxy::PROPERTY_ENABLE_COLLISION);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_TYPE);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_RADIUS);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_LENGTH);
      RemoveProperty(TransformableActorProxy::PROPERTY_COLLISION_BOX);
   }
}
