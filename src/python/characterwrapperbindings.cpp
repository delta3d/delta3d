#include <python/dtpython.h>
#include <dtAnim/characterwrapper.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>

using namespace boost::python;
using namespace dtAnim;

void init_CharacterWrapperBindings()
{
   void (CharacterWrapper::*SetGroundClamp1)(dtCore::Transformable*,float) = &CharacterWrapper::SetGroundClamp;
   void (CharacterWrapper::*SetGroundClamp2)(dtCore::Scene*,float) = &CharacterWrapper::SetGroundClamp;


   class_<CharacterWrapper, bases<dtCore::Transformable>, dtCore::RefPtr<CharacterWrapper>, boost::noncopyable>("CharacterWrapper", init<const std::string&>())
      .def("Update", &CharacterWrapper::Update)
      .def("GetHeightAboveGround", &CharacterWrapper::GetHeightAboveGround)
      .def("SetHeightAboveGround", &CharacterWrapper::SetHeightAboveGround)
      .def("SetGroundClamp", SetGroundClamp1)
      .def("SetGroundClamp", SetGroundClamp2)
      .def("GetSpeed", &CharacterWrapper::GetSpeed)
      .def("SetSpeed", &CharacterWrapper::SetSpeed)
      .def("GetRotationSpeed", &CharacterWrapper::GetRotationSpeed)
      .def("SetRotationSpeed", &CharacterWrapper::SetRotationSpeed)
      .def("RotateToHeading", &CharacterWrapper::RotateToHeading)
      .def("RotateToPoint", &CharacterWrapper::RotateToPoint)
      .def("GetLocalOffset", &CharacterWrapper::GetLocalOffset)
      .def("SetLocalOffset", &CharacterWrapper::SetLocalOffset)
      .def("GetHeading", &CharacterWrapper::GetHeading)
      .def("SetHeading", &CharacterWrapper::SetHeading)
      .def("PlayAnimation", &CharacterWrapper::PlayAnimation)
      .def("ClearAnimation", &CharacterWrapper::ClearAnimation)
      .def("ClearAllAnimations", &CharacterWrapper::ClearAllAnimations)
      .def("IsAnimationPlaying", &CharacterWrapper::IsAnimationPlaying)
      ;
}
