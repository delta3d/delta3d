// skyboxbindings.cpp: SkyBox binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/skybox.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

void initSkyBoxBindings()
{
   SkyBox* (*SkyBoxGI1)(int) = &SkyBox::GetInstance;
   SkyBox* (*SkyBoxGI2)(std::string) = &SkyBox::GetInstance;

   scope SkyBox_scope = class_<SkyBox, bases<EnvEffect>, dtCore::RefPtr<SkyBox> >("SkyBox", init<optional<std::string> >())
      .def("GetInstanceCount", &SkyBox::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", SkyBoxGI1, return_internal_reference<>())
      .def("GetInstance", SkyBoxGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetTexture", &SkyBox::SetTexture);
      
   enum_<SkyBox::SkyBoxSideEnum>("SkyBoxSideEnum")
      .value("SKYBOX_FRONT", SkyBox::SKYBOX_FRONT)
      .value("SKYBOX_RIGHT", SkyBox::SKYBOX_RIGHT)
      .value("SKYBOX_BACK", SkyBox::SKYBOX_BACK)
      .value("SKYBOX_LEFT", SkyBox::SKYBOX_LEFT)
      .value("SKYBOX_TOP", SkyBox::SKYBOX_TOP)
      .value("SKYBOX_BOTTOM", SkyBox::SKYBOX_BOTTOM)
      .export_values();
}
