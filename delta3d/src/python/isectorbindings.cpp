// isectorbindings.cpp: Isector binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/isector.h"

using namespace boost::python;
using namespace dtCore;

void initIsectorBindings()
{
   Isector* (*IsectorGI1)(int) = &Isector::GetInstance;
   Isector* (*IsectorGI2)(std::string) = &Isector::GetInstance;

   class_<Isector, bases<Transformable>, osg::ref_ptr<Isector> >("Isector", init<>())
      .def("GetInstanceCount", &Isector::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", IsectorGI1, return_internal_reference<>())
      .def("GetInstance", IsectorGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetLength", &Isector::SetLength)
      .def("GetHitPoint", &Isector::GetHitPoint)
      .def("SetDirection", &Isector::SetDirection)
      .def("SetStartPosition", &Isector::SetStartPosition)
      .def("Update", &Isector::Update)
      .def("SetGeometry", &Isector::SetGeometry);
}
