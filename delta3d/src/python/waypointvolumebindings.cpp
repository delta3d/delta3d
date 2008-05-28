#include <python/dtpython.h>
#include <dtActors/waypointvolumeactor.h>

using namespace boost::python;
using namespace dtActors;

void initWaypointVolumeBindings()
{
   WaypointVolumeActor* (*VolumeGI1)(int) = &WaypointVolumeActor::GetInstance;
   WaypointVolumeActor* (*VolumeGI2)(std::string) = &WaypointVolumeActor::GetInstance;  
   
   class_< WaypointVolumeActor, bases<dtCore::DeltaDrawable>, dtCore::RefPtr<WaypointVolumeActor>, boost::noncopyable >( "WaypointVolumeActor", init< optional< const std::string& > >() )
      .def( "GetInstanceCount", &WaypointVolumeActor::GetInstanceCount )
      .staticmethod("GetInstanceCount" )
      .def( "GetInstance", VolumeGI1, return_internal_reference<>() )
      .def( "GetInstance", VolumeGI2, return_internal_reference<>() )
      .staticmethod("GetInstance" )     
      .def( "GetWaypointSpacing", &WaypointVolumeActor::GetWaypointSpacing )
      .def( "IsPointInVolume", &WaypointVolumeActor::IsPointInVolume )
      ;
}
