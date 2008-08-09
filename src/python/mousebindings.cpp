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
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleButtonPressed" ).ptr(), boost::ref(mouse), button );
      #else
      return this->get_override( "HandleButtonPressed" )( boost::ref(mouse), button );
      #endif
   }
   
   bool HandleButtonReleased(const Mouse* mouse, Mouse::MouseButton button)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleButtonReleased" ).ptr(), boost::ref(mouse), button );
      #else
      return this->get_override( "HandleButtonReleased" )( boost::ref(mouse), button );
      #endif
   }
   
   bool HandleButtonClicked(const Mouse* mouse, Mouse::MouseButton button, int clickCount)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleButtonClicked" ).ptr(), boost::ref(mouse), button, clickCount );
      #else
      return this->get_override( "HandleButtonClicked" )( boost::ref(mouse), button, clickCount );
      #endif
   }
   
   bool HandleMouseMoved(const Mouse* mouse, float x, float y)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleMouseMoved" ).ptr(), boost::ref(mouse), x, y );
      #else
      return this->get_override( "HandleMouseMoved" )( boost::ref(mouse), x, y );
      #endif
   }
   
   bool HandleMouseDragged(const Mouse* mouse, float x, float y)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleMouseDragged" ).ptr(), boost::ref(mouse), x, y );
      #else
      return this->get_override( "HandleMouseDragged" )( boost::ref(mouse), x, y );
      #endif
   }
   
   bool HandleMouseScrolled(const Mouse* mouse, int delta)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleMouseScrolled" ).ptr(), boost::ref(mouse), delta );
      #else
      return this->get_override( "HandleMouseScrolled" )( boost::ref(mouse), delta );
      #endif
   }
};

void initMouseBindings()
{
   Mouse* (*MouseGI1)(int) = &Mouse::GetInstance;
   Mouse* (*MouseGI2)(std::string) = &Mouse::GetInstance;

   // Need wrapper for GetPosition that do not use assignment by reference. Python's
   // numeric types are immutable, and cannot have their values changed by reference.
   
   osg::Vec2 (Mouse::*GetPosition1)() const = &Mouse::GetPosition;
   void (Mouse::*SetPosition1)(float, float) = &Mouse::SetPosition;
   void (Mouse::*SetPosition2)(const osg::Vec2&) = &Mouse::SetPosition;

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
      .def("GetPosition", GetPosition1)
      .def("SetPosition", SetPosition1)
      .def("SetPosition", SetPosition2)
      .def("GetButtonState", &Mouse::GetButtonState)
      .def("AddMouseListener", &Mouse::AddMouseListener, with_custodian_and_ward<1,2>())
      .def("RemoveMouseListener", &Mouse::RemoveMouseListener);
      
   enum_<Mouse::MouseButton>("MouseButton")
      .value("LeftButton", Mouse::LeftButton)
      .value("MiddleButton", Mouse::MiddleButton)
      .value("RightButton", Mouse::RightButton)
      .export_values();
}
