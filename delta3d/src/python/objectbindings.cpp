// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/object.h"

using namespace boost::python;
using namespace dtCore;

class ObjectWrap : public Object
{
   public:

      ObjectWrap(PyObject* self, std::string name = "")
         : mSelf(self)
      {}

      void LoadFileWrapper1(std::string filename, bool useCache)
      {
         Object::LoadFile(filename,useCache);
      }
   
      void LoadFileWrapper2(std::string filename)
      {
         Object::LoadFile(filename);
      }
      
   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RGUL_overloads, RecenterGeometryUponLoad, 0, 1)

void initObjectBindings()
{
   Object* (*ObjectGI1)(int) = &Object::GetInstance;
   Object* (*ObjectGI2)(std::string) = &Object::GetInstance;

   class_<Object, bases<Physical>, osg::ref_ptr<ObjectWrap>, boost::noncopyable>("Object", init<optional<std::string> >())
      .def("GetInstanceCount", &Object::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ObjectGI1, return_internal_reference<>())
      .def("GetInstance", ObjectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &ObjectWrap::LoadFileWrapper1 )
      .def("LoadFile", &ObjectWrap::LoadFileWrapper2 )
      .def("RecenterGeometryUponLoad", &Object::RecenterGeometryUponLoad, RGUL_overloads());
}
