#include "componentgameactor.h"

#include "textlabelcomponent.h"

#include <dtCore/exceptionenum.h>
#include <dtCore/intactorproperty.h>

#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>

////////////////////////////////////////////////////////////////////////////////
void ComponentGameActor::Interaction()
{
   TextLabelComponent* tcomp;
   GetComponent(tcomp);
   tcomp->SetFlashing(!tcomp->IsFlashing());
}


////////////////////////////////////////////////////////////////////////////////
void ComponentGameActor::BuildPropertyMap()
{
   dtActors::GameMeshActor::BuildPropertyMap();  

   static const dtUtil::RefString GROUPNAME = "ComponentGameActor";

   AddProperty(new dtCore::IntActorProperty("Some Property", "Some Property",
            dtCore::IntActorProperty::SetFuncType(this, &ComponentGameActor::SetSomeProperty),
            dtCore::IntActorProperty::GetFuncType(this, &ComponentGameActor::GetSomeProperty),
            "", GROUPNAME));
}

void ComponentGameActor::BuildActorComponents()
{
   AddComponent(*new TextLabelComponent());

}
