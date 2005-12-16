
#include "python/dtpython.h"
#include <dtUtil/noisetexture.h>


void initNoiseTextureBindings()
{
   class_<NoiseTexture, NoiseTexture*, boost::noncopyable >("NoiseTexture", init<>())
      .def(init<int, int, double, double, int, optional<int> >())
      .def("GetInstanceCount", &Character::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CharacterGI1, return_internal_reference<>())
      .def("GetInstance", CharacterGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &CharacterWrap::LoadFileWrapper1)
      .def("LoadFile", &CharacterWrap::LoadFileWrapper2)
      .def("SetRotation", &Character::SetRotation)
      .def("GetRotation", &Character::GetRotation)
      .def("SetVelocity", &Character::SetVelocity)
      .def("GetVelocity", &Character::GetVelocity)
      .def("ExecuteAction", &Character::ExecuteAction, EA_overloads())
      .def("ExecuteActionWithSpeed", &Character::ExecuteActionWithSpeed, EAWS_overloads())
      .def("ExecuteActionWithAngle", &Character::ExecuteActionWithAngle, EAWA_overloads())
      .def("ExecuteActionWithSpeedAndAngle", &Character::ExecuteActionWithSpeedAndAngle, EAWSAA_overloads())
      .def("StopAction", &Character::StopAction);
}