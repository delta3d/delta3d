// deltawinbindings.cpp: DeltaWin binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "deltawin.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SC_overloads, ShowCursor, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SFSM_overloads, SetFullScreenMode, 0, 1)
   
void initDeltaWinBindings()
{
   DeltaWin* (*DeltaWinGI1)(int) = &DeltaWin::GetInstance;
   DeltaWin* (*DeltaWinGI2)(std::string) = &DeltaWin::GetInstance;

   bool (*DeltaWinCSR1)(int,int,int,int) = &DeltaWin::ChangeScreenResolution;
   bool (*DeltaWinCSR2)(Resolution) = &DeltaWin::ChangeScreenResolution;

   class_<Resolution>("Resolution")
            .def_readwrite("width", &Resolution::width)
            .def_readwrite("height", &Resolution::height)
            .def_readwrite("bitDepth", &Resolution::bitDepth)
            .def_readwrite("refresh", &Resolution::refresh);
   
   class_<DeltaWin, bases<Base>, osg::ref_ptr<DeltaWin> >("DeltaWin", init<optional<std::string, int, int, int, int> >())
      .def("GetInstanceCount", &DeltaWin::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", DeltaWinGI1, return_internal_reference<>())
      .def("GetInstance", DeltaWinGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CalcPixelCoords", &DeltaWin::CalcPixelCoords)
      .def("ShowCursor", &DeltaWin::ShowCursor, SC_overloads())
      .def("GetShowCursor", &DeltaWin::GetShowCursor)
      .def("SetFullScreenMode", &DeltaWin::SetFullScreenMode, SFSM_overloads())
      .def("GetFullScreenMode", &DeltaWin::GetFullScreenMode)
      .def("SetWindowTitle", &DeltaWin::SetWindowTitle)
      .def("GetWindowTitle", &DeltaWin::GetWindowTitle)
      .def("SetPosition", &DeltaWin::SetPosition)
      .def("GetPosition", &DeltaWin::GetPosition)
      .def("GetRenderSurface", &DeltaWin::GetRenderSurface, return_internal_reference<>())
      .def("GetKeyboard", &DeltaWin::GetKeyboard, return_internal_reference<>())
      .def("GetMouse", &DeltaWin::GetMouse, return_internal_reference<>())
      .def("GetCurrentResolution", &DeltaWin::GetCurrentResolution)
      .def("ChangeScreenResolution", DeltaWinCSR1)
      .def("ChangeScreenResolution", DeltaWinCSR2)
      .def("IsValidResolution", &DeltaWin::IsValidResolution);
}
