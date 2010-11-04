// clouddomebindings.cpp: CloudDome binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/clouddome.h>
#include <dtCore/scene.h>
#include <osg/Geode>

using namespace boost::python;
using namespace dtCore;

void initCloudDomeBindings()
{
   CloudDome* (*CloudDomeGI1)(int) = &CloudDome::GetInstance;
   CloudDome* (*CloudDomeGI2)(std::string) = &CloudDome::GetInstance;

   class_<CloudDome, bases<EnvEffect>, dtCore::RefPtr<CloudDome>, boost::noncopyable >("CloudDome", init<int, int, float, float, float, float, float, int>())
      .def(init<float,int,const std::string&>())
      .def("GetInstanceCount", &CloudDome::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CloudDomeGI1, return_internal_reference<>())
      .def("GetInstance", CloudDomeGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetScale", &CloudDome::GetScale)
      .def("GetExponent", &CloudDome::GetExponent)
      .def("GetCutoff", &CloudDome::GetCutoff)
      .def("GetSpeedX", &CloudDome::GetSpeedX)
      .def("GetSpeedY", &CloudDome::GetSpeedY)
      .def("GetBias", &CloudDome::GetBias)
      .def("GetCloudColor", &CloudDome::GetCloudColor)
      .def("GetEnable", &CloudDome::GetEnable)
      .def("SetScale", &CloudDome::SetScale)
      .def("SetExponent", &CloudDome::SetExponent)
      .def("SetCutoff", &CloudDome::SetCutoff)
      .def("SetSpeedX", &CloudDome::SetSpeedX)
      .def("SetSpeedY", &CloudDome::SetSpeedY)
      .def("SetBias", &CloudDome::SetBias)
      .def("SetCloudColor", &CloudDome::SetCloudColor)
      .def("SetShaderEnable", &CloudDome::SetShaderEnable);
}
