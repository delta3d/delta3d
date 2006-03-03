// deltawinbindings.cpp: DeltaWin binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/deltawin.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SC_overloads, ShowCursor, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SFSM_overloads, SetFullScreenMode, 0, 1)

int IVR1( const DeltaWin::ResolutionVec& rv, int w) { return DeltaWin::IsValidResolution(rv, w); }
int IVR2( const DeltaWin::ResolutionVec& rv, int w, int h) { return DeltaWin::IsValidResolution(rv, w, h); }
int IVR3( const DeltaWin::ResolutionVec& rv, int w, int h, int r) { return DeltaWin::IsValidResolution(rv, w, h, r); }
int IVR4( const DeltaWin::ResolutionVec& rv, int w, int h, int r, int d) { return DeltaWin::IsValidResolution(rv, w, h, r, d); }


void initDeltaWinBindings()
{
   DeltaWin* (*DeltaWinGI1)(int) = &DeltaWin::GetInstance;
   DeltaWin* (*DeltaWinGI2)(std::string) = &DeltaWin::GetInstance;

   bool (*DeltaWinCSR1)(int,int,int,int) = &DeltaWin::ChangeScreenResolution;
   bool (*DeltaWinCSR2)(DeltaWin::Resolution) = &DeltaWin::ChangeScreenResolution;

   void (DeltaWin::*GP1)( int&, int&, int&, int& ) = &DeltaWin::GetPosition;
   Keyboard* (DeltaWin::*GK1)() = &DeltaWin::GetKeyboard;
   Mouse* (DeltaWin::*GM1)() = &DeltaWin::GetMouse;

   scope DeltaWinScope = class_<DeltaWin, bases<Base>, dtCore::RefPtr<DeltaWin> >("DeltaWin")
      .def(init<optional<const std::string&, int, int, int, int, bool, bool> >())
      .def(init<const std::string&,dtCore::Keyboard*,dtCore::Mouse*>())
      .def("GetInstanceCount", &DeltaWin::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", DeltaWinGI1, return_internal_reference<>())
      .def("GetInstance", DeltaWinGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CalcPixelCoords", &DeltaWin::CalcPixelCoords)
      .def("CalcWindowCoords", &DeltaWin::CalcWindowCoords)
      .def("ShowCursor", &DeltaWin::ShowCursor, SC_overloads())
      .def("GetShowCursor", &DeltaWin::GetShowCursor)
      .def("SetFullScreenMode", &DeltaWin::SetFullScreenMode, SFSM_overloads())
      .def("GetFullScreenMode", &DeltaWin::GetFullScreenMode)
      .def("KillGLWindow", &DeltaWin::KillGLWindow)
      .def("SetWindowTitle", &DeltaWin::SetWindowTitle)
      .def("GetWindowTitle", &DeltaWin::GetWindowTitle, return_internal_reference<>())
      .def("SetPosition", &DeltaWin::SetPosition)
      .def("GetPosition", GP1)
      .def("GetKeyboard", GK1, return_internal_reference<>())
      .def("GetMouse", GM1, return_internal_reference<>())
      .def("SetKeyboard",&DeltaWin::SetKeyboard)
      .def("SetMouse",&DeltaWin::SetMouse)
      .def("GetCurrentResolution", &DeltaWin::GetCurrentResolution)
      .def("ChangeScreenResolution", DeltaWinCSR1)
      .def("ChangeScreenResolution", DeltaWinCSR2)
      .def("IsValidResolution", &DeltaWin::IsValidResolution)
      .def("IsValidResolution", IVR1)
      .def("IsValidResolution", IVR2)
      .def("IsValidResolution", IVR3)
      .def("IsValidResolution", IVR4);

   class_<DeltaWin::Resolution>("Resolution")
      .def_readwrite("width", &DeltaWin::Resolution::width)
      .def_readwrite("height", &DeltaWin::Resolution::height)
      .def_readwrite("bitDepth", &DeltaWin::Resolution::bitDepth)
      .def_readwrite("refresh", &DeltaWin::Resolution::refresh);

}
