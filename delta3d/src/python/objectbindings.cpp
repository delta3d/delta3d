// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/object.h"

using namespace boost::python;
using namespace dtCore;
/*
class ObjectWrap : public Object
{
   public:

      ObjectWrap(PyObject* self, std::string name = "")
         : Object(name),
           mSelf(self)
      {}

      virtual bool LoadFile(std::string filename, bool useCache = true)
      {
         return call_method<bool>(mSelf, "LoadFile", filename, useCache);
      }
      
      bool DefaultLoadFile(std::string filename, bool useCache)
      {
         return Object::LoadFile(filename, useCache);
      }
      
   protected:

      PyObject* mSelf;
};
*/
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initObjectBindings()
{
   Object* (*ObjectGI1)(int) = &Object::GetInstance;
   Object* (*ObjectGI2)(std::string) = &Object::GetInstance;

   class_<Object, bases<Physical>, osg::ref_ptr<Object>, boost::noncopyable>("Object", init<optional<std::string> >())
      .def("GetInstanceCount", &Object::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ObjectGI1, return_internal_reference<>())
      .def("GetInstance", ObjectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &Object::LoadFile, LF_overloads()[return_internal_reference<>()]);
}
