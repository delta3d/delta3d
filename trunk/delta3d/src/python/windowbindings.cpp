// windowbindings.cpp: Window binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "window.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SC_overloads, ShowCursor, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SFSM_overloads, SetFullScreenMode, 0, 1)

void initWindowBindings()
{
   Window* (*WindowGI1)(int) = &Window::GetInstance;
   Window* (*WindowGI2)(std::string) = &Window::GetInstance;

   class_<Window, bases<Base> >("Window", init<optional<std::string, int, int, int, int> >())
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
      .def("ChangeScreenResolution", &Window::ChangeScreenResolution);
}