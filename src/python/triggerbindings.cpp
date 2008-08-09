#include <python/dtpython.h>
#include <dtABC/trigger.h>

using namespace boost::python;
using namespace dtABC;

void initTriggerBindings()
{
   Trigger* (*TriggerGI1)(int) = &Trigger::GetInstance;
   Trigger* (*TriggerGI2)(std::string) = &Trigger::GetInstance;

   Action* (Trigger::*GetAction1)() = &Trigger::GetAction;

   class_< Trigger, bases<dtCore::DeltaDrawable>, dtCore::RefPtr<Trigger>, boost::noncopyable >( "Trigger", init< optional< const std::string& > >() )
      .def( "GetInstanceCount", &Trigger::GetInstanceCount )
      .staticmethod("GetInstanceCount" )
      .def( "GetInstance", TriggerGI1, return_internal_reference<>() )
      .def( "GetInstance", TriggerGI2, return_internal_reference<>() )
      .staticmethod("GetInstance" )
      .def( "SetEnabled", &Trigger::SetEnabled )
      .def( "GetEnabled", &Trigger::GetEnabled )
      .def( "SetTimeDelay", &Trigger::SetTimeDelay )
      .def( "GetTimeDelay", &Trigger::GetTimeDelay )
      .def( "GetTimeLeft", &Trigger::GetTimeLeft )
      .def( "Fire", &Trigger::Fire )
      .def( "SetAction", &Trigger::SetAction )
      .def( "GetAction", GetAction1, return_internal_reference<>() )
      .def( "GetTimesActive", &Trigger::GetTimesActive )
      .def( "SetTimesActive", &Trigger::SetTimesActive )
      ;
}
