#include <dtActors/actionactorproxy.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>

namespace dtActors
{
   void ActionActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "Action";

      dtABC::Action *a = static_cast<dtABC::Action*>(GetActor());

      AddProperty(new dtDAL::FloatActorProperty("Time Step","Time Step",
         dtDAL::MakeFunctor(*a,&dtABC::Action::SetTimeStep),
         dtDAL::MakeFunctorRet(*a,&dtABC::Action::GetTimeStep),
         "Sets the time step which will be the increment in seconds to update this Action by.", GROUPNAME));

      AddProperty(new dtDAL::BooleanActorProperty("Tick Once Per Frame","Tick Once Per Frame",
         dtDAL::MakeFunctor(*a,&dtABC::Action::SetTickOncePerFrame),
         dtDAL::MakeFunctorRet(*a,&dtABC::Action::GetTickOncePerFrame),
         "Sets whether OnNextStep should be called once per frame or stepped multiple times over a time period", GROUPNAME));
   } 
}
