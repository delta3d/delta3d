// deltawinbindings.cpp: DeltaWin binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/deltawin.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <osgViewer/GraphicsWindow>

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

   bool (DeltaWin::*CalcPixelCoords1) (const osg::Vec2 &, osg::Vec2 &) const = &DeltaWin::CalcPixelCoords;
   bool (DeltaWin::*CalcWindowCoords1)(const osg::Vec2 &, osg::Vec2 &) const = &DeltaWin::CalcWindowCoords;

   bool (*DeltaWinCSR1)(int,int,int,int) = &DeltaWin::ChangeScreenResolution;
   bool (*DeltaWinCSR2)(DeltaWin::Resolution) = &DeltaWin::ChangeScreenResolution;

   void (DeltaWin::*SetPosition1)(const DeltaWin::PositionSize&) = &DeltaWin::SetPosition;
   void (DeltaWin::*SetPosition2)(int, int, int, int) = &DeltaWin::SetPosition;
   DeltaWin::PositionSize (DeltaWin::*GetPosition1)() const = &DeltaWin::GetPosition;

   osgViewer::GraphicsWindow* (DeltaWin::*GetOsgViewerGraphicsWindow1)() = &DeltaWin::GetOsgViewerGraphicsWindow;


   scope DeltaWinScope = class_<DeltaWin, bases<Base>, dtCore::RefPtr<DeltaWin>, boost::noncopyable >("DeltaWin")
      .def(init<optional<const dtCore::DeltaWin::DeltaWinTraits&> >())
      .def(init<const std::string&,osgViewer::GraphicsWindow&>())
      .def("GetInstanceCount", &DeltaWin::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", DeltaWinGI1, return_internal_reference<>())
      .def("GetInstance", DeltaWinGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("CalcPixelCoords", CalcPixelCoords1)
      .def("CalcWindowCoords", CalcWindowCoords1)
      .def("ShowCursor", &DeltaWin::ShowCursor, SC_overloads())
      .def("GetShowCursor", &DeltaWin::GetShowCursor)
      .def("SetFullScreenMode", &DeltaWin::SetFullScreenMode, SFSM_overloads())
      .def("GetFullScreenMode", &DeltaWin::GetFullScreenMode)
      .def("KillGLWindow", &DeltaWin::KillGLWindow)
      .def("SetWindowTitle", &DeltaWin::SetWindowTitle)
      // This used to work when GetWindowTitle returned a reference
      //.def("GetWindowTitle", &DeltaWin::GetWindowTitle, return_internal_reference<>())
      .def("GetWindowTitle", &DeltaWin::GetWindowTitle)
      .def("SetPosition", SetPosition1)
      .def("SetPosition", SetPosition2)
      .def("GetPosition", GetPosition1)
      .def("GetCurrentResolution", &DeltaWin::GetCurrentResolution)
      .def("ChangeScreenResolution", DeltaWinCSR1)
      .def("ChangeScreenResolution", DeltaWinCSR2)
      .def("IsValidResolution", IVR1)
      .def("IsValidResolution", IVR2)
      .def("IsValidResolution", IVR3)
      .def("IsValidResolution", IVR4)
      .def("GetOsgViewerGraphicsWindow", GetOsgViewerGraphicsWindow1, return_internal_reference<>())
      ;

   class_<DeltaWin::PositionSize>("PositionSize")
      .def_readwrite("mX", &DeltaWin::PositionSize::mX)
      .def_readwrite("mY", &DeltaWin::PositionSize::mY)
      .def_readwrite("mWidth", &DeltaWin::PositionSize::mWidth)
      .def_readwrite("mHeight", &DeltaWin::PositionSize::mHeight)
      ;

   class_<DeltaWin::Resolution>("Resolution")
      .def_readwrite("width", &DeltaWin::Resolution::width)
      .def_readwrite("height", &DeltaWin::Resolution::height)
      .def_readwrite("bitDepth", &DeltaWin::Resolution::bitDepth)
      .def_readwrite("refresh", &DeltaWin::Resolution::refresh)
      ;

}
