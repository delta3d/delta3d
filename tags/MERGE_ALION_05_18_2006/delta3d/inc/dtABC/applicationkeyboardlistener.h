/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
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
* @author John K. Grant
*/
#ifndef DELTA_APPLICATION_KEYBOARD_LISTENER
#define DELTA_APPLICATION_KEYBOARD_LISTENER 

#include <dtCore/keyboard.h>   // for base class
#include <dtCore/refptr.h>     // for member
#include <dtABC/export.h>

namespace dtABC
{
   class BaseABC;

   /// Provides application callback support.
   class DT_ABC_EXPORT ApplicationKeyboardListener : public dtCore::KeyboardListener
   {
   public:
      typedef dtCore::KeyboardListener BaseClass;

      ApplicationKeyboardListener();
      ApplicationKeyboardListener(dtABC::BaseABC* app);

      void SetApplication(dtABC::BaseABC* app);
      const dtABC::BaseABC* GetApplication() const { return mApplication; }

      // inherited methods
      bool HandleKeyPressed(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc);
      bool HandleKeyReleased(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc);
      bool HandleKeyTyped(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc);

   protected:
      ~ApplicationKeyboardListener();

      dtABC::BaseABC* mApplication;
   };
}

#endif // DELTA_APPLICATION_KEYBOARD_LISTENER
