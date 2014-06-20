/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006 MOVES Institute 
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
 * John K. Grant
 */
#ifndef DELTA_CEGUI_KEYBOARD_LISTENER
#define DELTA_CEGUI_KEYBOARD_LISTENER

#include <dtCore/keyboard.h>   // for base class
#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <CEGUI/CEGUIInputEvent.h>  // for internal type, CEGUI::Key::Scan
#include <CEGUI/CEGUIVersion.h>
DT_DISABLE_WARNING_END
#include <dtGUI/export.h>                   // for export symbols

namespace dtGUI
{
   class DT_GUI_EXPORT CEGUIKeyboardListener : public dtCore::KeyboardListener
   {
   public:
      CEGUIKeyboardListener();

      bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);
      bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

      ///\todo what is this supposed to do?  inject both keypressed and keyreleased to CEGUI::System?
      /// Haven't those already been injected with the other calls?
      bool HandleKeyTyped(const dtCore::Keyboard* keyboard, int key);

      /**
       * Determines the CEGUI scancode that corresponds to the specified Producer::KeyboardKey.
       *
       * @param key the key to map
       * @return the corresponding CEGUI key scancode
       */
      static CEGUI::Key::Scan KeyboardKeyToKeyScan(int key);

   protected:
      virtual ~CEGUIKeyboardListener();

   private:
      void MakeCurrent();
   };
}

#endif  // DELTA_CEGUI_KEYBOARD_LISTENER
