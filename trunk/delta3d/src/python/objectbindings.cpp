// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/object.h>

using namespace boost::python;
using namespace dtCore;

// We need to wrap Object so LoadFile does not return an osg::Node
// since Python doesn't know what one is!
class ObjectWrap : public Object
{
   public:

      ObjectWrap(PyObject* self, const std::string& name = "")
         : mSelf(self)
      {}

      void LoadFileWrapper1(const std::string& filename, bool useCache)
      {
         Object::LoadFile(filename,useCache);
      }
   
      void LoadFileWrapper2(const std::string& filename)
      {
         Object::LoadFile(filename);
      }
      
   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RGUL_overloads, RecenterGeometryUponLoad, 0, 1)

void initObjectBindings()
{
   Object* (*ObjectGI1)(int) = &Object::GetInstance;
   Object* (*ObjectGI2)(std::string) = &Object::GetInstance;

   // ObjectWrap* should probably be a dtCore::RefPtr<ObjectWrap>, but that causes a big fat crash
   // on exit on Linux: *** glibc detected *** python: double free or corruption (out): 0xb64054d0 ***
   class_<Object, bases<Physical>, ObjectWrap*, boost::noncopyable>("Object", init<optional<const std::string&> >())
      .def("GetInstanceCount", &Object::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ObjectGI1, return_internal_reference<>())
      .def("GetInstance", ObjectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &ObjectWrap::LoadFileWrapper1 )
      .def("LoadFile", &ObjectWrap::LoadFileWrapper2 )
      .def("RecenterGeometryUponLoad", &Object::RecenterGeometryUponLoad, RGUL_overloads());
}
