#include "[!output PROJECT_NAME]Input.h"
#include <dtABC/application.h>

[!output PROJECT_NAME]Input::[!output PROJECT_NAME]Input():
dtGame::BaseInputComponent("Input")
{

}

[!output PROJECT_NAME]Input::~[!output PROJECT_NAME]Input()
{

}

bool [!output PROJECT_NAME]Input::HandleKeyPressed( const dtCore::Keyboard* keyboard, int key )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleKeyReleased( const dtCore::Keyboard* keyboard, int key )
{
   bool handled = true;
   switch(key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
      {
         dtABC::Application& app = GetGameManager()->GetApplication();
         app.Quit();
      }
      break;

   default:
      {
         handled = false;
      }
      break;
   }

   return handled;
}

bool [!output PROJECT_NAME]Input::HandleKeyTyped( const dtCore::Keyboard* keyboard, int key )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleButtonPressed( const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleButtonReleased( const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleButtonClicked( const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleMouseMoved( const dtCore::Mouse* mouse, float x, float y )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleMouseDragged( const dtCore::Mouse* mouse, float x, float y )
{
   return false;
}

bool [!output PROJECT_NAME]Input::HandleMouseScrolled( const dtCore::Mouse* mouse, int delta )
{
   return false;
}
