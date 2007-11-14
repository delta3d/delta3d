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
#ifndef DELTA_GENERIC_KEYBOARD_LISTENER
#define DELTA_GENERIC_KEYBOARD_LISTENER

#include <osgGA/GUIEventAdapter>

#include <dtCore/keyboard.h>   // for base class
#include <dtCore/export.h>     // for export symbols
#include <dtUtil/functor.h>    // for generic callback adapter members

namespace dtCore
{
   /// Provides generic callback support for Keyboard events.
   class DT_CORE_EXPORT GenericKeyboardListener : public KeyboardListener
   {
   public:
      typedef KeyboardListener BaseClass;
      typedef dtUtil::Functor<bool,TYPELIST_2(const Keyboard*,int)> CallbackType;

      GenericKeyboardListener();

      // -- inherited methods -- //
      bool HandleKeyPressed(const Keyboard* kb, int kc);
      bool HandleKeyReleased(const Keyboard* kb, int kc);
      bool HandleKeyTyped(const Keyboard* kb, int kc);

      // -- generic members -- //

      void SetPressedCallback(const CallbackType& callback);
      const CallbackType& GetPressedCallback() const;
      bool IsPressedCallbackEnabled() const;
      void DisablePressedCallback();

      void SetReleasedCallback(const CallbackType& callback);
      const CallbackType& GetReleasedCallback() const;
      bool IsReleasedCallbackEnabled() const;
      void DisableReleasedCallback();

      void SetTypedCallback(const CallbackType& callback);
      const CallbackType& GetTypedCallback() const;
      bool IsTypedCallbackEnabled() const;
      void DisableTypedCallback();

   protected:
      ~GenericKeyboardListener();

   private:
      CallbackType mPressedCB, mReleasedCB, mTypedCB;
      bool mPressedEnabled, mReleasedEnabled, mTypedEnabled;
   };
}

#endif // DELTA_GENERIC_KEYBOARD_LISTENER
