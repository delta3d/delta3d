// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "object.h"

using namespace boost::python;
using namespace dtCore;

class ObjectWrap : public Object
{
   public:

      ObjectWrap(PyObject* self, std::string name = "")
         : Object(name),
           mSelf(self)
      {}

      /*
      virtual osg::Node* GetOSGNode()
      {
         return call_method<osg::Node*>(mSelf, "GetOSGNode"); 
      }

      osg::Node* DefaultGetOSGNode()
      {
         return Object::GetOSGNode();
      }
      */
      
      virtual bool LoadFile(std::string filename, bool useCache = true)
      {
         return call_method<bool>(mSelf, "LoadFile", filename, useCache);
      }
      
      bool LoadFile1(std::string filename)
      {
         LoadFile(filename);
      }
      
      bool DefaultLoadFile1(std::string filename)
      {
         return Object::LoadFile(filename);
      }
      
      bool LoadFile2(std::string filename, bool useCache)
      {
         LoadFile(filename, useCache);
      }
      
      bool DefaultLoadFile2(std::string filename, bool useCache)
      {
         return Object::LoadFile(filename, useCache);
      }
      
      virtual std::string GetFilename() const
      {
         return call_method<std::string>(mSelf, "GetFilename");
      }
      
      std::string DefaultGetFilename() const
      {
         return Object::GetFilename();
      }
      
   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initObjectBindings()
{
   Object* (*ObjectGI1)(int) = &Object::GetInstance;
   Object* (*ObjectGI2)(std::string) = &Object::GetInstance;

   class_<Object, bases<Transformable, Drawable, Physical>, osg::ref_ptr<ObjectWrap>, boost::noncopyable>("Object", init<optional<std::string> >())
      .def("GetInstanceCount", &Object::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", ObjectGI1, return_internal_reference<>())
      .def("GetInstance", ObjectGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      //.def("GetOSGNode", &Object::GetOSGNode, &ObjectWrap::DefaultGetOSGNode, return_internal_reference<>())
      .def("LoadFile", &Object::LoadFile, LF_overloads())
      .def("LoadFile", &Object::LoadFile, &ObjectWrap::DefaultLoadFile2)
      .def("GetFilename", &Object::GetFilename, &ObjectWrap::DefaultGetFilename);
}