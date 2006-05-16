// mousebindings.cpp: Mouse binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/mouse.h>

using namespace boost::python;
using namespace dtCore;

void initMouseBindings()
{
   Mouse* (*MouseGI1)(int) = &Mouse::GetInstance;
   Mouse* (*MouseGI2)(std::string) = &Mouse::GetInstance;

   typedef void (Mouse::*MouseFloatFloatMemFun)(float&,float&) const;
   MouseFloatFloatMemFun MouseGP1 = &Mouse::GetPosition;

   class_<MouseListener, dtCore::RefPtr<MouseListener>, boost::noncopyable >("MouseListener",no_init)
      .def("HandleButtonPressed", pure_virtual(&MouseListener::HandleButtonPressed))
      .def("HandleButtonReleased", pure_virtual(&MouseListener::HandleButtonReleased))
      .def("HandleButtonClicked", pure_virtual(&MouseListener::HandleButtonClicked))
      .def("HandleMouseMoved", pure_virtual(&MouseListener::HandleMouseMoved))
      .def("HandleMouseDragged", pure_virtual(&MouseListener::HandleMouseDragged))
      .def("HandleMouseScrolled", pure_virtual(&MouseListener::HandleMouseScrolled));

   scope in_mouse = class_<Mouse, bases<InputDevice>, dtCore::RefPtr<Mouse> >("Mouse", no_init)
      .def("GetInstanceCount", &Mouse::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", MouseGI1, return_internal_reference<>())
      .def("GetInstance", MouseGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetPosition", MouseGP1)
      .def("SetPosition", &Mouse::SetPosition)
      .def("GetButtonState", &Mouse::GetButtonState)
      .def("AddMouseListener", &Mouse::AddMouseListener)
      .def("RemoveMouseListener", &Mouse::RemoveMouseListener);
      
   enum_<Mouse::MouseButton>("MouseButton")
      .value("LeftButton", Mouse::LeftButton)
      .value("MiddleButton", Mouse::MiddleButton)
      .value("RightButton", Mouse::RightButton)
      .export_values();
}
