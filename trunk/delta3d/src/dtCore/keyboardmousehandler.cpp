#include <prefix/dtcoreprefix-src.h>
#include <dtCore/keyboardmousehandler.h>

#include <osgGA/GUIEventAdapter>


namespace dtCore {

////////////////////////////////////////////////////////////////////////////////

KeyboardMouseHandler::KeyboardMouseHandler()
   : mKeyboard(new Keyboard)
   , mMouse(new Mouse)
{
}


KeyboardMouseHandler::KeyboardMouseHandler(dtCore::View* view)
   : mView(view)
   , mKeyboard(new Keyboard)
   , mMouse(new Mouse(view))
{
}


KeyboardMouseHandler::KeyboardMouseHandler(Keyboard* keyboard, Mouse* mouse)
   : mKeyboard(keyboard)
   , mMouse(mouse)
{
}


KeyboardMouseHandler::~KeyboardMouseHandler()
{
}


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


void KeyboardMouseHandler::SetView(dtCore::View* view)
{
   mView = view;
   if (mMouse.valid())
   {
      mMouse->SetView(mView.get());
   }
}


bool KeyboardMouseHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
   //returned values in the range of (-1..1)
   float x = ea.getXnormalized();
   float y = ea.getYnormalized();

   switch (ea.getEventType() )
   {
   case osgGA::GUIEventAdapter::SCROLL:
      return (mMouse->MouseScroll(ea.getScrollingMotion()));
      break;

   case osgGA::GUIEventAdapter::DRAG:
      return (mMouse->MouseMotion(x, y));
      break;
   case osgGA::GUIEventAdapter::MOVE:
      return (mMouse->PassiveMouseMotion(x, y));
      break;

   case osgGA::GUIEventAdapter::PUSH:
      if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      {
         return (mMouse->ButtonDown(x, y, Mouse::LeftButton));
      }
      else if (ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
      {
         return (mMouse->ButtonDown(x, y, Mouse::MiddleButton));
      }
      else if (ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
      {
         return (mMouse->ButtonDown(x, y, Mouse::RightButton));
      }
      break;

   case osgGA::GUIEventAdapter::RELEASE:
      if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      {
         return (mMouse->ButtonUp(x, y, Mouse::LeftButton));
      }
      else if (ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
      {
         return (mMouse->ButtonUp(x, y, Mouse::MiddleButton));
      }
      else if (ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
      {
         return (mMouse->ButtonUp(x, y, Mouse::RightButton));
      }
      break;

   case osgGA::GUIEventAdapter::DOUBLECLICK:
      if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      {
         return (mMouse->DoubleButtonDown(x, y, Mouse::LeftButton));
      }
      else if (ea.getButton() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
      {
         return (mMouse->DoubleButtonDown(x, y, Mouse::MiddleButton));
      }
      else if (ea.getButton() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
      {
         return (mMouse->DoubleButtonDown(x, y, Mouse::RightButton));
      }
      break;

   case osgGA::GUIEventAdapter::KEYDOWN:
      return (mKeyboard->KeyDown(ea.getKey()));
      break;

   case osgGA::GUIEventAdapter::KEYUP:
      return (mKeyboard->KeyUp(ea.getKey()));
      break;

   default:
      return (false);
   }

   return (false);
}

//////////////////////////////////////////////////////////////////////////
void KeyboardMouseHandler::SetKeyboard(Keyboard* kb)
{
   mKeyboard = kb;
}

//////////////////////////////////////////////////////////////////////////
Keyboard* KeyboardMouseHandler::GetKeyboard()
{
   return mKeyboard.get();
}

//////////////////////////////////////////////////////////////////////////
const Keyboard* KeyboardMouseHandler::GetKeyboard() const
{
   return mKeyboard.get();
}

//////////////////////////////////////////////////////////////////////////
Mouse* KeyboardMouseHandler::GetMouse()
{
   return mMouse.get();
}

//////////////////////////////////////////////////////////////////////////
const Mouse* KeyboardMouseHandler::GetMouse() const
{
   return mMouse.get();
}
////////////////////////////////////////////////////////////////////////////////

} // namespace dtCore
