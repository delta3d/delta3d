#include <dtActors/actionactorproxy.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>


using namespace dtABC;
using namespace dtDAL;

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

   } 

}
