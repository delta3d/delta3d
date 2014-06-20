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
 * John K. Grant
 */
#ifndef DELTA_CEGUI_MOUSE_LISTENER
#define DELTA_CEGUI_MOUSE_LISTENER

#include <dtCore/mouse.h>  // for base class
#include <dtGUI/export.h>                   // for export symbols
#include <CEGUI/CEGUIVersion.h>
namespace dtGUI
{


   /// A mouse device listener to inject input to CEGUI.
   ///\todo Why maintain mWidth AND mHalfWidth?
   class DT_GUI_EXPORT CEGUIMouseListener : public dtCore::MouseListener
   {
   public:
      CEGUIMouseListener();
      void SetWindowSize(unsigned int width, unsigned int height);

      ///\todo test System for null, throw exception if null.
      bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y);
      ///\todo test System for null, throw exception if null.
      bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);
      ///\todo test System for null, throw exception if null.
      bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      ///\todo test System for null, throw exception if null.
      bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      ///\todo test System for null, throw exception if null.
      bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

      ///\todo what is this supposed to do?  inject both buttonpressed and buttonreleased to CEGUI::System?
      /// Haven't those already been injected with the other calls?
      bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount);

   protected:
      ~CEGUIMouseListener();

   private:
      void UpdateWindowSize();
      void MakeCurrent();

      unsigned int   mWidth; ///<the width of the Window
      unsigned int   mHeight; ///<The height of the Window
      unsigned int   mHalfWidth; ///<the width of the Window
      unsigned int   mHalfHeight; ///<The height of the Window
      float mMouseX; ///<The current Mouse X position
      float mMouseY; ///<the current Mouse Y position
   };
}

#endif  // DELTA_CEGUI_MOUSE_LISTENER

