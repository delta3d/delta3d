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

   void LoadFileWrapper1(std::string filename, bool useCache)
   {
      Loadable::LoadFile(filename,useCache);
   }
   
   void LoadFileWrapper2(std::string filename)
   {
      Loadable::LoadFile(filename);
   }

protected:

   PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initLoadableBindings()
{
   class_<Loadable, LoadableWrap*, boost::noncopyable>("Loadable", no_init)
      //.def("LoadFile", &Loadable::LoadFile, LF_overloads()[return_internal_reference<>()])
      //.def("LoadFile", &Loadable::LoadFile, &LoadableWrap::LoadFile[return_internal_reference<>()])
      .def("LoadFile", &LoadableWrap::LoadFileWrapper1 )
      .def("LoadFile", &LoadableWrap::LoadFileWrapper2 )
      .def("GetFilename", &Loadable::GetFilename );
}
