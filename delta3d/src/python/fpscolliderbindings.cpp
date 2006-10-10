#include <python/dtpython.h>
#include <dtCore/fpscollider.h>
#include <osg/Vec3>
#include <osg/Matrix>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

void initFPSColliderBindings()
{

  scope colliderScope = class_<FPSCollider>("FPSCollider", init<float, float, float, float, Scene*>())
      .def("Update", &FPSCollider::Update)
      .def("GetSlideThreshold", &FPSCollider::GetSlideThreshold)
      .def("SetSlideThreshold", &FPSCollider::SetSlideThreshold)
      .def("GetSlideSpeed", &FPSCollider::GetSlideSpeed)
      .def("SetSlideSpeed", &FPSCollider::SetSlideSpeed)
      .def("GetJumpSpeed", &FPSCollider::GetJumpSpeed)
      .def("SetJumpSpeed", &FPSCollider::SetJumpSpeed)
      .def("GetHeightAboveTerrain", &FPSCollider::GetHeightAboveTerrain)
      .def("SetDimensions", &FPSCollider::SetDimensions)
      .def("GetMode", &FPSCollider::GetMode)
   ;

  enum_<FPSCollider::eMode>("eMode")
     .value("WALKING", FPSCollider::WALKING)
     .value("FALLING", FPSCollider::FALLING)
     .value("SLIDING", FPSCollider::SLIDING)
     .export_values();
}
