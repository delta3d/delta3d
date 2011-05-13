#include "componentgameactor.h"

#include "textlabelcomponent.h"

#include <dtDAL/exceptionenum.h>
#include <dtDAL/intactorproperty.h>

#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>

////////////////////////////////////////////////////////////////////////////////
ComponentGameActor::ComponentGameActor(dtGame::GameActorProxy& proxy)
: dtActors::GameMeshActor(proxy)
{
}

////////////////////////////////////////////////////////////////////////////////
void ComponentGameActor::Interaction()
{
   TextLabelComponent* tcomp;
   GetComponent(tcomp);
   tcomp->SetFlashing(!tcomp->IsFlashing());
}


////////////////////////////////////////////////////////////////////////////////
void ComponentGameActorProxy::BuildPropertyMap()
{
   dtActors::GameMeshActorProxy::BuildPropertyMap();  

   ComponentGameActor* actor = static_cast<ComponentGameActor*>(GetActor());

   static const std::string GROUPNAME = "ComponentGameActor";

   AddProperty(new dtDAL::IntActorProperty("Some Property", "Some Property",
            dtDAL::IntActorProperty::SetFuncType(actor, &ComponentGameActor::SetSomeProperty),
            dtDAL::IntActorProperty::GetFuncType(actor, &ComponentGameActor::GetSomeProperty),
            "", GROUPNAME));
}


////////////////////////////////////////////////////////////////////////////////
void ComponentGameActorProxy::CreateActor() 
{
   ComponentGameActor* actor = new ComponentGameActor(*this);

   // add component to the actor. When done now, the component's properties
   // are accessible in STAGE.
   actor->AddComponent(*new TextLabelComponent());

   SetActor(*actor); 
}
