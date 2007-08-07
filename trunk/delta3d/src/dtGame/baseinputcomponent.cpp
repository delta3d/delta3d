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
#include <prefix/dtgameprefix-src.h>
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
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   InputComponentKeyboardListener::InputComponentKeyboardListener(dtGame::BaseInputComponent &inputComp) : 
      mInputComp(&inputComp)
   {
   }

   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////
   BaseInputComponent::BaseInputComponent(const std::string &name) :
      dtGame::GMComponent(name)
   {
      mMouseListener = new InputComponentMouseListener(*this);
      mKeyboardListener = new InputComponentKeyboardListener(*this);
   }

   ////////////////////////////////////////////////////////////////////
   void BaseInputComponent::OnAddedToGM() 
   { 
      //enable the keyboard input.
      dtCore::DeltaWin* win = GetGameManager()->GetApplication().GetWindow();
      win->GetMouse()->AddMouseListener(mMouseListener.get());
      win->GetKeyboard()->AddKeyboardListener(mKeyboardListener.get());
   }

   ////////////////////////////////////////////////////////////////////
   void BaseInputComponent::OnRemovedFromGM()
   {
      dtCore::DeltaWin* win = GetGameManager()->GetApplication().GetWindow();
      win->GetMouse()->RemoveMouseListener(mMouseListener.get());
      win->GetKeyboard()->RemoveKeyboardListener(mKeyboardListener.get());
   }

   ////////////////////////////////////////////////////////////////////
   bool BaseInputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard,
      Producer::KeyboardKey key, Producer::KeyCharacter character)
   {
      // the default case handles the escape key to quit.  Override this to abort this behavior
      bool handled = true;
      switch(key)
      {
         case Producer::Key_Escape:
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
}
