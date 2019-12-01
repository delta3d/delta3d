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

   LoadableWrap(PyObject* self, const std::string& name = "")
      : mSelf(self)
   {}

   void LoadFileWrapper1(const std::string& filename, bool useCache)
   {
      Loadable::LoadFile(filename,useCache);
   }
   
   void LoadFileWrapper2(const std::string& filename)
   {
      Loadable::LoadFile(filename);
   }

protected:

   PyObject* mSelf;
};

void initLoadableBindings()
{
   class_<Loadable, LoadableWrap*, boost::noncopyable>("Loadable", no_init)
      .def("LoadFile", &LoadableWrap::LoadFileWrapper1 )
      .def("LoadFile", &LoadableWrap::LoadFileWrapper2 )
      .def("GetFilename", &Loadable::GetFilename, return_value_policy<copy_const_reference>() );
}
