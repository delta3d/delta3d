/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/baseinputcomponent.h>
#include <dtABC/application.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/deltawin.h>

namespace dtGame
{
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   InputComponentMouseListener::InputComponentMouseListener(dtGame::BaseInputComponent &inputComp) : 
      mInputComp(&inputComp)
   {
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentMouseListener::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleButtonPressed(mouse, button);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentMouseListener::HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleButtonReleased(mouse, button);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentMouseListener::HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleButtonClicked(mouse, button, clickCount);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentMouseListener::HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleMouseMoved(mouse, x, y);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentMouseListener::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleMouseDragged(mouse, x, y);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentMouseListener::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleMouseScrolled(mouse, delta);
   }

   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   InputComponentKeyboardListener::InputComponentKeyboardListener(dtGame::BaseInputComponent& inputComp) :
      mInputComp(&inputComp)
   {
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentKeyboardListener::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleKeyPressed(keyboard, key);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
   {
      if (!mInputComp.valid())
         return false;

      return mInputComp->HandleKeyReleased(keyboard, key);
   }

   ////////////////////////////////////////////////////////////////////
   bool InputComponentKeyboardListener::HandleKeyTyped(const dtCore::Keyboard* keyboard, int key )
   {
      if (!mInputComp.valid())
         return false;

      return false;
   }

   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   BaseInputComponent::BaseInputComponent(dtCore::SystemComponentType& type) :
      dtGame::GMComponent(type)
   {
      SetName(DEFAULT_NAME);

      mMouseListener = new InputComponentMouseListener(*this);
      mKeyboardListener = new InputComponentKeyboardListener(*this);
   }

   ////////////////////////////////////////////////////////////////////
   void BaseInputComponent::OnAddedToGM()
   { 
      //enable the keyboard input.
      dtCore::View* view = GetGameManager()->GetApplication().GetView();
      view->GetMouse()->AddMouseListener(mMouseListener.get());
      view->GetKeyboard()->AddKeyboardListener(mKeyboardListener.get());
   }

   ////////////////////////////////////////////////////////////////////
   void BaseInputComponent::OnRemovedFromGM()
   {
      dtCore::View* view = GetGameManager()->GetApplication().GetView();
      view->GetMouse()->RemoveMouseListener(mMouseListener.get());
      view->GetKeyboard()->RemoveKeyboardListener(mKeyboardListener.get());
   }

   ////////////////////////////////////////////////////////////////////
   bool BaseInputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      // the default case handles the escape key to quit.  Override this to abort this behavior
      bool handled = true;
      switch (key)
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

   ////////////////////////////////////////////////////////////////////
   void BaseInputComponent::ConvertToKeypadEquivalent(int& key)
   {
      switch(key)
      {
      case osgGA::GUIEventAdapter::KEY_0:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_0;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_1:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_1;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_2:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_2;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_3:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_3;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_4:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_4;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_5:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_5;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_6:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_6;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_7:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_7;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_8:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_8;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_9:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_9;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Left:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Left;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Right:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Right;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Up:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Up;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Down:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Down;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Page_Up:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Page_Down:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Begin:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Begin;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_End:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_End;
          break;
      }
      case osgGA::GUIEventAdapter::KEY_Backslash:
      {
          key = osgGA::GUIEventAdapter::KEY_KP_Insert;
          break;
      }
      default:
         break;
      }

   }
}
