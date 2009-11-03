#include <python/dtpython.h>
#include <dtCore/view.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <osgViewer/View>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GMPO_overloads, GetMousePickedObject, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GMPP_overloads, GetMousePickPosition, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GPP_overloads, GetPickPosition, 2, 3)

void initViewBindings()
{
   Camera* (View::*GetCamera1)() = &View::GetCamera;
   const Camera* (View::*GetCamera2)() const = &View::GetCamera;

   Scene* (View::*GetScene1)() = &View::GetScene;
   const Scene* (View::*GetScene2)() const = &View::GetScene;

   Keyboard* (View::*GetKeyboard1)() = &View::GetKeyboard;
   const Keyboard* (View::*GetKeyboard2)() const = &View::GetKeyboard;

   Mouse* (View::*GetMouse1)() = &View::GetMouse;
   const Mouse* (View::*GetMouse2)() const = &View::GetMouse;

   class_<View, bases<Base>, dtCore::RefPtr<View>, boost::noncopyable>("View", init<optional<const std::string&,bool> >())
      .def(init<osgViewer::View*,optional<const std::string&,bool> >())
      .def("SetFrameBin", &View::SetFrameBin)
      .def("GetFrameBin", &View::GetFrameBin)
      .def("SetRenderOrder", &View::SetRenderOrder)
      .def("GetRenderOrder", &View::GetRenderOrder)
      .def("AddSlave", &View::AddSlave)
      .def("SetCamera", &View::SetCamera)
      .def("GetCamera", GetCamera1, return_internal_reference<>())
      .def("GetCamera", GetCamera2, return_internal_reference<>())
      .def("SetScene", &View::SetScene)
      .def("GetScene", GetScene1, return_internal_reference<>())
      .def("GetScene", GetScene2, return_internal_reference<>())
      // omitting GetOsgViewerView...what could you do with it if you got it?
      .def("SetKeyboard", &View::SetKeyboard)
      .def("GetKeyboard", GetKeyboard1, return_internal_reference<>())
      .def("GetKeyboard", GetKeyboard2, return_internal_reference<>())
      .def("SetMouse", &View::SetMouse)
      .def("GetMouse", GetMouse1, return_internal_reference<>())
      .def("GetMouse", GetMouse2, return_internal_reference<>())
	   .def( "GetMousePickedObject", &View::GetMousePickedObject, GMPO_overloads()[ return_internal_reference<>() ] )
	   .def( "GetMousePickPosition", &View::GetMousePickPosition, GMPP_overloads() )
	   .def( "GetPickPosition", &View::GetPickPosition, GPP_overloads() )
      // omitted Get and SetDatabasePager
      ;
}
