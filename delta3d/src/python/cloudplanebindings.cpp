// cloudplanebindings.cpp: CloudPlane binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "cloudplane.h"

using namespace boost::python;
using namespace dtCore;

void initCloudPlaneBindings()
{
   CloudPlane* (*CloudPlaneGI1)(int) = &CloudPlane::GetInstance;
   CloudPlane* (*CloudPlaneGI2)(std::string) = &CloudPlane::GetInstance;

   class_<CloudPlane, bases<EnvEffect>, osg::ref_ptr<CloudPlane> >("CloudPlane", init<int, float, int, float, float, float, int, float, optional<std::string> >())
      .def("GetInstanceCount", &CloudPlane::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CloudPlaneGI1, return_internal_reference<>())
      .def("GetInstance", CloudPlaneGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("getHeight", &CloudPlane::getHeight);
}
