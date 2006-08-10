#include <python/dtpython.h>
#include <dtABC/proximitytrigger.h>

using namespace boost::python;
using namespace dtABC;

void initProximityTriggerBindings()
{
   ProximityTrigger* (*ProximityTriggerGI1)(int) = &ProximityTrigger::GetInstance;
   ProximityTrigger* (*ProximityTriggerGI2)(std::string) = &ProximityTrigger::GetInstance;

   Trigger* (ProximityTrigger::*GT1)() = &ProximityTrigger::GetTrigger;

   class_< ProximityTrigger, bases<dtCore::Transformable>, dtCore::RefPtr<ProximityTrigger>, boost::noncopyable >( "ProximityTrigger", init< optional< const std::string& > >() )
      .def( "GetInstanceCount", &ProximityTrigger::GetInstanceCount )
      .staticmethod( "GetInstanceCount" )
      .def( "GetInstance", ProximityTriggerGI1, return_internal_reference<>() )
      .def( "GetInstance", ProximityTriggerGI2, return_internal_reference<>() )
      .staticmethod("GetInstance" )
      .def( "GetTrigger", GT1, return_internal_reference<>() )
      ;
}
