// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/loadable.h"

using namespace boost::python;
using namespace dtCore;

/*
class LoadableWrap : public Loadable
{
public:

   LoadableWrap(PyObject* self, std::string name = "")
      : mSelf(self)
   {}

   virtual osg::Node* LoadFile(std::string filename, bool useCache )
   {
      return call_method<osg::Node*>(mSelf, "LoadFile", filename, useCache);
   }

   osg::Node* DefaultLoadFile(std::string filename)
   {
      return Loadable::LoadFile(filename);
   }

protected:

   PyObject* mSelf;
};

*/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initLoadableBindings()
{
   class_<Loadable, Loadable*, boost::noncopyable>("Loadable", no_init)
      .def("LoadFile", &Loadable::LoadFile, LF_overloads()[return_internal_reference<>()])
      //.def("LoadFile", &Loadable::LoadFile, &LoadableWrap::LoadFile[return_internal_reference<>()])
      .def("GetFilename", &Loadable::GetFilename );
}
