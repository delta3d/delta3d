// dtguiflbindings.cpp: Python bindings for gui_fl library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <gui_fl/guimgr.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(S_overloads, Show, 0, 1)

BOOST_PYTHON_MODULE(PyDtGUIFl)
{
   class_<GUI, boost::noncopyable>("GUI")
      .def("Show", &GUI::Show, S_overloads())
      .def("IsShown", &GUI::IsShown);
}
