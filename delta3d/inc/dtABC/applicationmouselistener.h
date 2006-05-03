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
#ifndef DELTA_APPLICATION_MOUSE_LISTENER
#define DELTA_APPLICATION_MOUSE_LISTENER

#include <dtCore/mouse.h>                // for base class
#include <dtCore/refptr.h>               // for member
#include <dtABC/export.h>

namespace dtABC
{
   class BaseABC;

   /// Provides application callback support.
   class DT_ABC_EXPORT ApplicationMouseListener : public dtCore::MouseListener
   {
   public:
      typedef dtCore::MouseListener BaseClass;

      ApplicationMouseListener();
      ApplicationMouseListener(dtABC::BaseABC* app);

      void SetApplication(dtABC::BaseABC* app);
      const dtABC::BaseABC* GetApplication() const { return mApplication.get(); }

      // inherited methods
      bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount);
      bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y);
      bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);
      bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

   protected:
      ~ApplicationMouseListener();

   private:
      dtCore::RefPtr<dtABC::BaseABC> mApplication;
   };
}

#endif  // DELTA_APPLICATION_MOUSE_LISTENER
