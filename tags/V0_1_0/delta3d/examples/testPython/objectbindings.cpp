// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "pythonbindings.h"
#include "object.h"

using namespace boost::python;
using namespace P51;

void initObjectBindings()
{
   Object* (*ObjectGI1)(int) = &Object::GetInstance;
   Object* (*ObjectGI2)(std::string) = &Object::GetInstance;

   class_<Object, bases<Transformable, Drawable> >("Object", init<optional<std::string> >())
      .def("GetInstanceCount", &Object::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ObjectGI1, return_internal_reference<>())
      .def("GetInstance", ObjectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &Object::LoadFile)
      .def("GetFilename", &Object::GetFilename);
}