#include "componentgameactor.h"

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include "textlabelcomponent.h"


ComponentGameActor::ComponentGameActor(dtGame::GameActorProxy &proxy)
: dtActors::GameMeshActor(proxy)
{
}

void ComponentGameActor::Interaction()
{
   TextLabelComponent* tcomp;
   GetComponent(tcomp);
   tcomp->SetFlashing(!tcomp->IsFlashing());
}


// -----------------------------------




/// Builds the actor properties
void ComponentGameActorProxy::BuildPropertyMap()
{
   dtActors::GameMeshActorProxy::BuildPropertyMap();  

   ComponentGameActor* actor = static_cast<ComponentGameActor*>(GetActor());

   static const std::string GROUPNAME = "ComponentGameActor";

   AddProperty(new dtDAL::IntActorProperty("Some Property", "Some Property",
      dtDAL::MakeFunctor(*actor, &ComponentGameActor::SetSomeProperty),
      dtDAL::MakeFunctorRet(*actor, &ComponentGameActor::GetSomeProperty),
      "", GROUPNAME));
}


/// Instantiates the actor
void ComponentGameActorProxy::CreateActor() 
{
   ComponentGameActor* actor = new ComponentGameActor(*this);

   // add component to the actor. When done now, the component's properties
   // are accessible in STAGE.
   actor->AddComponent(new TextLabelComponent());

   SetActor(*actor); 
}
