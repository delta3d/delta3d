#include <dtActors/actionactorproxy.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/functor.h>

namespace dtActors
{
   void ActionActorProxy::BuildPropertyMap()
   {
      dtCore::BaseActorObject::BuildPropertyMap();

      const std::string GROUPNAME = "Action";

      dtABC::Action* a = static_cast<dtABC::Action*>(GetDrawable());

      AddProperty(new dtCore::FloatActorProperty("Time Step","Time Step",
         dtCore::FloatActorProperty::SetFuncType(a,&dtABC::Action::SetTimeStep),
         dtCore::FloatActorProperty::GetFuncType(a,&dtABC::Action::GetTimeStep),
         "Sets the time step which will be the increment in seconds to update this Action by.", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty("Tick Once Per Frame","Tick Once Per Frame",
         dtCore::BooleanActorProperty::SetFuncType(a,&dtABC::Action::SetTickOncePerFrame),
         dtCore::BooleanActorProperty::GetFuncType(a,&dtABC::Action::GetTickOncePerFrame),
         "Sets whether OnNextStep should be called once per frame or stepped multiple times over a time period", GROUPNAME));
   }
}
