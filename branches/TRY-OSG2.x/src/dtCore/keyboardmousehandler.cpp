#include <prefix/dtcoreprefix-src.h>
#include <dtCore/keyboardmousehandler.h>

#include <osgGA/GUIEventAdapter>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>//due to include of scene.h
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/keyboard.h>//due to include of scene.h


namespace dtCore
{

KeyboardMouseHandler::KeyboardMouseHandler()
: mKeyboard(new Keyboard), mMouse(new Mouse)
{
}

KeyboardMouseHandler::KeyboardMouseHandler(dtCore::View * view)
:   mView(view), mKeyboard(new Keyboard), mMouse(new Mouse(view))
{}

KeyboardMouseHandler::KeyboardMouseHandler(Keyboard* keyboard, Mouse* mouse)
:   mKeyboard(keyboard), mMouse(mouse)
{}

KeyboardMouseHandler::~KeyboardMouseHandler()
{}

void KeyboardMouseHandler::SetMouse(Mouse* m)
{
   if (mMouse.valid())
   {
      mMouse->SetView(NULL);
   }
   
   mMouse = m;
   
   if (mMouse.valid())
   {
      mMouse->SetView(mView.get());
   }
}

void KeyboardMouseHandler::SetView(dtCore::View * view)
{
   mView = view;
   if (mMouse.valid())
   {
      mMouse->SetView(mView.get());
   }
}

Keyboard * KeyboardMouseHandler::CreateKeyboard()
{
   mKeyboard =  new Keyboard;
   return mKeyboard.get();
}
Mouse * KeyboardMouseHandler::CreateMouse()
{
   mMouse = new Mouse(mView.get());
   return mMouse.get();
}

bool KeyboardMouseHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *, osg::NodeVisitor *)
{
   float x = (ea.getX() - (ea.getWindowWidth()/2)) / (ea.getWindowWidth() ? ea.getWindowWidth() : 1);
   float y = (ea.getY() - (ea.getWindowHeight()/2)) / (ea.getWindowHeight() ? ea.getWindowHeight() : 1);
   
   switch (ea.getEventType() )
   {
      case osgGA::GUIEventAdapter::SCROLL :
         return (mMouse->MouseScroll(ea.getScrollingMotion()));
         break;

      case osgGA::GUIEventAdapter::DRAG :
         return (mMouse->MouseMotion(x, y));
         break;
      case osgGA::GUIEventAdapter::MOVE :
         return (mMouse->PassiveMouseMotion(x, y));
         break;

      case osgGA::GUIEventAdapter::PUSH :
         if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            return (mMouse->ButtonDown(x, y, Mouse::LeftButton));
         else if (ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
            return (mMouse->ButtonDown(x, y, Mouse::MiddleButton));
         else if (ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
            return (mMouse->ButtonDown(x, y, Mouse::RightButton));

      case osgGA::GUIEventAdapter::RELEASE :
         if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            return (mMouse->ButtonUp(x, y, Mouse::LeftButton));
         else if (ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
            return (mMouse->ButtonUp(x, y, Mouse::MiddleButton));
         else if (ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
            return (mMouse->ButtonUp(x, y, Mouse::RightButton));

      case osgGA::GUIEventAdapter::DOUBLECLICK :
         if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            return (mMouse->DoubleButtonDown(x, y, Mouse::LeftButton));
         else if (ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
            return (mMouse->DoubleButtonDown(x, y, Mouse::MiddleButton));
         else if (ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
            return (mMouse->DoubleButtonDown(x, y, Mouse::RightButton));

      case osgGA::GUIEventAdapter::KEYDOWN :
         return (mKeyboard->KeyDown(ea.getKey()));

      case osgGA::GUIEventAdapter::KEYUP :
         return (mKeyboard->KeyUp(ea.getKey()));
      default:
         return (false);
   }    
}
}
