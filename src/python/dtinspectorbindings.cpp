// dtinspectorbindings.cpp: Python bindings for dtInspector library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <dtInspector/inspector.h>

using namespace boost::python;
using namespace dtCore;
using namespace dtInspector;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(S_overloads, Show, 0, 1)

BOOST_PYTHON_MODULE(PyDtInspector)
{
   class_<Inspector, boost::noncopyable>("Inspector")
      .def("Show", &Inspector::Show, S_overloads())
      .def("IsShown", &Inspector::IsShown);
}
