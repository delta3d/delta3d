// mousebindings.cpp: Mouse binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "mouse.h"

using namespace boost::python;
using namespace dtCore;

class MouseListenerWrap : public MouseListener
{
   public:
   
      MouseListenerWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void ButtonPressed(Mouse* mouse, MouseButton button)
      {
         call_method<void>(mSelf, "ButtonPressed", mouse, button);
      }
      
      void DefaultButtonPressed(Mouse* mouse, MouseButton button)
      {
         MouseListener::ButtonPressed(mouse, button);
      }
      
      virtual void ButtonReleased(Mouse* mouse, MouseButton button)
      {
         call_method<void>(mSelf, "ButtonReleased", mouse, button);
      }
      
      void DefaultButtonReleased(Mouse* mouse, MouseButton button)
      {
         MouseListener::ButtonReleased(mouse, button);
      }
      
      virtual void ButtonClicked(Mouse* mouse, MouseButton button, int clickCount)
      {
         call_method<void>(mSelf, "ButtonClicked", mouse, button, clickCount);
      }
      
      void DefaultButtonClicked(Mouse* mouse, MouseButton button, int clickCount)
      {
         MouseListener::ButtonClicked(mouse, button, clickCount);
      }
      
      virtual void MouseMoved(Mouse* mouse, float x, float y)
      {
         call_method<void>(mSelf, "MouseMoved", mouse, x, y);
      }
      
      void DefaultMouseMoved(Mouse* mouse, float x, float y)
      {
         MouseListener::MouseMoved(mouse, x, y);
      }
      
      virtual void MouseDragged(Mouse* mouse, float x, float y)
      {
         call_method<void>(mSelf, "MouseDragged", mouse, x, y);
      }
      
      void DefaultMouseDragged(Mouse* mouse, float x, float y)
      {
         MouseListener::MouseDragged(mouse, x, y);
      }
      
      virtual void MouseScrolled(Mouse* mouse, int delta)
      {
         call_method<void>(mSelf, "MouseScrolled", mouse, delta);
      }
      
      void DefaultMouseScrolled(Mouse* mouse, int delta)
      {
         MouseListener::MouseScrolled(mouse, delta);
      }
      
   private:
      
      PyObject* mSelf;
};

void initMouseBindings()
{
   Mouse* (*MouseGI1)(int) = &Mouse::GetInstance;
   Mouse* (*MouseGI2)(std::string) = &Mouse::GetInstance;

   enum_<MouseButton>("MouseButton")
      .value("LeftButton", LeftButton)
      .value("MiddleButton", MiddleButton)
      .value("RightButton", RightButton)
      .export_values();
      
   class_<Mouse, bases<InputDevice>, osg::ref_ptr<Mouse> >("Mouse", no_init)
      .def("GetInstanceCount", &Mouse::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", MouseGI1, return_internal_reference<>())
      .def("GetInstance", MouseGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetPosition", &Mouse::GetPosition)
      .def("GetButtonState", &Mouse::GetButtonState)
      .def("AddMouseListener", &Mouse::AddMouseListener)
      .def("RemoveMouseListener", &Mouse::RemoveMouseListener);
      
   class_<MouseListener, MouseListenerWrap, boost::noncopyable>("MouseListener")
      .def("ButtonPressed", &MouseListener::ButtonPressed, &MouseListenerWrap::DefaultButtonPressed)
      .def("ButtonReleased", &MouseListener::ButtonReleased, &MouseListenerWrap::DefaultButtonReleased)
      .def("ButtonClicked", &MouseListener::ButtonClicked, &MouseListenerWrap::DefaultButtonClicked)
      .def("MouseMoved", &MouseListener::MouseMoved, &MouseListenerWrap::DefaultMouseMoved)
      .def("MouseDragged", &MouseListener::MouseDragged, &MouseListenerWrap::DefaultMouseDragged)
      .def("MouseScrolled", &MouseListener::MouseScrolled, &MouseListenerWrap::DefaultMouseScrolled);
}