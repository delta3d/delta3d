#include <dtGUI/ceguimouselistener.h>
#include <CEGUI/CEGUIInputEvent.h>       // for internal type, CEGUI::Key::Scan
#include <CEGUI/CEGUISystem.h>
#include <dtUtil/mathdefines.h>          // for fast math function

using namespace dtGUI;

CEGUIMouseListener::CEGUIMouseListener()
{
}

CEGUIMouseListener::~CEGUIMouseListener()
{
}

void CEGUIMouseListener::SetWindowSize(unsigned int width, unsigned int height)
{
   mWidth = width;
   mHeight = height;
   mHalfWidth = width/2;
   mHalfHeight = height/2;
}

bool CEGUIMouseListener::HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
{
   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;

   ///\todo document these magic constants from the CEUIDrawable-days.
   return CEGUI::System::getSingleton().injectMousePosition( ((x+1)*0.5f)*mWidth, ((-y+1)*0.5f)*mHeight);
}

bool CEGUIMouseListener::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   return HandleMouseMoved(mouse, x, y);
}

bool CEGUIMouseListener::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   switch( button )
   {
   case dtCore::Mouse::LeftButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
      break;
   case dtCore::Mouse::RightButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
      break;
   case dtCore::Mouse::MiddleButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
      break;
   }

   return false;
}

bool CEGUIMouseListener::HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   switch(button)
   {
   case dtCore::Mouse::LeftButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
      }  break;

   case dtCore::Mouse::RightButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
      }  break;

   case dtCore::Mouse::MiddleButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
      }  break;
   }

   return false;
}

bool CEGUIMouseListener::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
{
   return CEGUI::System::getSingleton().injectMouseWheelChange( (float)delta );
}

bool CEGUIMouseListener::HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
{
   return false;
}
