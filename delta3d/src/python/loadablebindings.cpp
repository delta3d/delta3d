// objectbindings.cpp: Object binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/loadable.h"

using namespace boost::python;
using namespace dtCore;

class LoadableWrap : public Loadable
{
public:

   LoadableWrap(PyObject* self, std::string name = "")
      : mSelf(self)
   {}

   virtual bool LoadFile(std::string filename, bool useCache = true)
   {
      return call_method<bool>(mSelf, "LoadFile", filename, useCache);
   }

   void DefaultLoadFile(std::string filename, bool useCache)
   {
      return Loadable::LoadFile(filename, useCache);
   }

protected:

   PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initLoadableBindings()
{
   class_<Loadable, Loadable*, boost::noncopyable>("Loadable", no_init)
      .def("LoadFile", &Loadable::LoadFile, LF_overloads())
      .def("LoadFile", &Loadable::LoadFile, &LoadableWrap::DefaultLoadFile)
      .def("GetFilename", &Loadable::GetFilename );
}
