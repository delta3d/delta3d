// windowbindings.cpp: Window binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "window.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SC_overloads, ShowCursor, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SFSM_overloads, SetFullScreenMode, 0, 1)

void initWindowBindings()
{
   Window* (*WindowGI1)(int) = &Window::GetInstance;
   Window* (*WindowGI2)(std::string) = &Window::GetInstance;

   class_<Window, bases<Base>, osg::ref_ptr<Window> >("Window", init<optional<std::string, int, int, int, int> >())
      .def("GetInstanceCount", &Window::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", WindowGI1, return_internal_reference<>())
      .def("GetInstance", WindowGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CalcPixelCoords", &Window::CalcPixelCoords)
      .def("ShowCursor", &Window::ShowCursor, SC_overloads())
      .def("GetShowCursor", &Window::GetShowCursor)
      .def("SetFullScreenMode", &Window::SetFullScreenMode, SFSM_overloads())
      .def("GetFullScreenMode", &Window::GetFullScreenMode)
      .def("SetWindowTitle", &Window::SetWindowTitle)
      .def("GetWindowTitle", &Window::GetWindowTitle)
      .def("SetPosition", &Window::SetPosition)
      .def("GetPosition", &Window::GetPosition)
      .def("GetRenderSurface", &Window::GetRenderSurface, return_internal_reference<>())
      .def("GetKeyboard", &Window::GetKeyboard, return_internal_reference<>())
      .def("GetMouse", &Window::GetMouse, return_internal_reference<>())
      .def("ChangeScreenResolution", &Window::ChangeScreenResolution);
}
