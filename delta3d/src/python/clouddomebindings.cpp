// clouddomebindings.cpp: CloudDome binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "clouddome.h"

using namespace boost::python;
using namespace dtCore;

void initCloudDomeBindings()
{
   CloudDome* (*CloudDomeGI1)(int) = &CloudDome::GetInstance;
   CloudDome* (*CloudDomeGI2)(std::string) = &CloudDome::GetInstance;

   class_<CloudDome, bases<EnvEffect>, osg::ref_ptr<CloudDome> >("CloudDome", init<int, int, float, float, float, float, float, int>())
      .def("GetInstanceCount", &CloudDome::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CloudDomeGI1, return_internal_reference<>())
      .def("GetInstance", CloudDomeGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("getScale", &CloudDome::getScale)
      .def("getExponent", &CloudDome::getExponent)
      .def("getCutoff", &CloudDome::getCutoff)
      .def("getSpeedX", &CloudDome::getSpeedX)
      .def("getSpeedY", &CloudDome::getSpeedY)
      .def("getBias", &CloudDome::getBias)
      .def("getCloudColor", &CloudDome::getCloudColor, return_internal_reference<>())
      .def("getEnable", &CloudDome::getEnable)
      .def("setScale", &CloudDome::setScale)
      .def("setExponent", &CloudDome::setExponent)
      .def("setCutoff", &CloudDome::setCutoff)
      .def("setSpeedX", &CloudDome::setSpeedX)
      .def("setSpeedY", &CloudDome::setSpeedY)
      .def("setBias", &CloudDome::setBias)
      .def("setCloudColor", &CloudDome::setCloudColor)
      .def("setShaderEnable", &CloudDome::setShaderEnable);
}