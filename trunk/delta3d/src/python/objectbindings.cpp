// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "object.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initObjectBindings()
{
   Object* (*ObjectGI1)(int) = &Object::GetInstance;
   Object* (*ObjectGI2)(std::string) = &Object::GetInstance;

   class_<Object, bases<Transformable, Drawable>, osg::ref_ptr<Object> >("Object", init<optional<std::string> >())
      .def("GetInstanceCount", &Object::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ObjectGI1, return_internal_reference<>())
      .def("GetInstance", ObjectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &Object::LoadFile, LF_overloads())
      .def("GetFilename", &Object::GetFilename);
}