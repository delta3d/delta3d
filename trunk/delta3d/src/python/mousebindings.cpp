// mousebindings.cpp: Mouse binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/mouse.h>

using namespace boost::python;
using namespace dtCore;

class MouseListenerWrap : public MouseListener, public wrapper<MouseListener>
{
   public:
   MouseListenerWrap()
   {
   }
   protected:
   virtual ~MouseListenerWrap()
   {
   }
   public:
   
   bool HandleButtonPressed(const Mouse* mouse, Mouse::MouseButton button)
   {
      return this->get_override( "HandleButtonPressed" )( boost::ref(mouse), button );
   }
   
   bool HandleButtonReleased(const Mouse* mouse, Mouse::MouseButton button)
   {
      return this->get_override( "HandleButtonReleased" )( boost::ref(mouse), button );
   }
   
   bool HandleButtonClicked(const Mouse* mouse, Mouse::MouseButton button, int clickCount)
   {
      return this->get_override( "HandleButtonClicked" )( boost::ref(mouse), button, clickCount );
   }
   
   bool HandleMouseMoved(const Mouse* mouse, float x, float y)
   {
      return this->get_override( "HandleMouseMoved" )( boost::ref(mouse), x, y );
   }
   
   bool HandleMouseDragged(const Mouse* mouse, float x, float y)
   {
      return this->get_override( "HandleMouseDragged" )( boost::ref(mouse), x, y );
   }
   
   bool HandleMouseScrolled(const Mouse* mouse, int delta)
   {
      return this->get_override( "HandleMouseScrolled" )( boost::ref(mouse), delta );
   }
};

void initMouseBindings()
{
   Mouse* (*MouseGI1)(int) = &Mouse::GetInstance;
   Mouse* (*MouseGI2)(std::string) = &Mouse::GetInstance;

   typedef void (Mouse::*MouseFloatFloatMemFun)(float&,float&) const;
   MouseFloatFloatMemFun MouseGP1 = &Mouse::GetPosition;

   class_<MouseListenerWrap, dtCore::RefPtr<MouseListenerWrap>, boost::noncopyable >("MouseListener")
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
      .def("AddMouseListener", &Mouse::AddMouseListener, with_custodian_and_ward<1,2>())
      .def("RemoveMouseListener", &Mouse::RemoveMouseListener);
      
   enum_<Mouse::MouseButton>("MouseButton")
      .value("LeftButton", Mouse::LeftButton)
      .value("MiddleButton", Mouse::MiddleButton)
      .value("RightButton", Mouse::RightButton)
      .export_values();
}
