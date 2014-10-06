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
#ifndef DELTA_GENERIC_MOUSE_LISTENER
#define DELTA_GENERIC_MOUSE_LISTENER

#include <dtCore/mouse.h>                // for base class
#include <dtCore/export.h>               // for symbols
#include <dtUtil/functor.h>              // for generic callback adapter members

namespace dtCore
{
   /// Provides generic callback support for Mouse events.

   /** This class is an optional MouseListener-derived class which can be used instead
     * of MouseListener.  GenericMouseListener uses a callback approach to listening
     * to Mouse events.  Instead of deriving a class from MouseListener and overwriting
     * the methods, you can create an instance of GenericMouseListener and supply callback
     * functions which will be called.
     *
     * To use this class, supply an instance to the mouse you wish to listen to.  Then 
     * add any callback functions to want to be triggered using:
     * - SetPressedCallback()
     * - SetReleasedCallback()
     * - SetClickedCallback()
     * - SetMovedCallback()
     * - SetDraggedCallback()
     * - SetScrolledCallback()
     *
     * @code
     * class Handler
     * {
     * public:
     *    bool Pressed(const dtCore::Mouse *mouse, dtCore::Mouse::MouseButton button);
     *    {
     *       return true;
     *    }
     * }; 
     * 
     * Handler *h = new Handler();
     *
     * RefPtr<GenericMouseListener> ml = new GenericMouseListener();
     * myMouse->AddMouseListener(ml.get());
     * 
     * ml->SetPressedCallback(GenericMouseListener::ButtonCallbackType(h, &Handler::Pressed));
     * @endcode
     */
   class DT_CORE_EXPORT GenericMouseListener : public MouseListener
   {
   public:
      typedef MouseListener BaseClass;
      typedef dtUtil::Functor<bool,TYPELIST_2(const Mouse*,Mouse::MouseButton)> ButtonCallbackType;
      typedef dtUtil::Functor<bool,TYPELIST_3(const Mouse*,Mouse::MouseButton,int)> ClickCallbackType;
      typedef dtUtil::Functor<bool,TYPELIST_3(const Mouse*,float,float)> MovementCallbackType;
      typedef dtUtil::Functor<bool,TYPELIST_2(const Mouse*,int)> WheelCallbackType;

      GenericMouseListener();

      // inherited methods
      virtual bool HandleButtonPressed(const Mouse* mouse, Mouse::MouseButton button);
      virtual bool HandleButtonReleased(const Mouse* mouse, Mouse::MouseButton button);
      virtual bool HandleButtonClicked(const Mouse* mouse, Mouse::MouseButton button, int clickCount);
      virtual bool HandleMouseMoved(const Mouse* mouse, float x, float y);
      virtual bool HandleMouseDragged(const Mouse* mouse, float x, float y);
      virtual bool HandleMouseScrolled(const Mouse* mouse, int delta);

      // -- callback support -- //
      void SetPressedCallback(const ButtonCallbackType& callback);
      const ButtonCallbackType& GetPressedCallback() const;
      bool IsPressedCallbackEnabled() const;
      void DisablePressedCallback();

      void SetReleasedCallback(const ButtonCallbackType& callback);
      const ButtonCallbackType& GetReleasedCallback() const;
      bool IsReleasedCallbackEnabled() const;
      void DisableReleasedCallback();

      void SetClickedCallback(const ClickCallbackType& callback);
      const ClickCallbackType& GetClickedCallback() const;
      bool IsClickedCallbackEnabled() const;
      void DisableClickedCallback();

      void SetMovedCallback(const MovementCallbackType& callback);
      const MovementCallbackType& GetMovedCallback() const;
      bool IsMovedCallbackEnabled() const;
      void DisableMovedCallback();

      void SetDraggedCallback(const MovementCallbackType& callback);
      const MovementCallbackType& GetDraggedCallback() const;
      bool IsDraggedCallbackEnabled() const;
      void DisableDraggedCallback();

      void SetScrolledCallback(const WheelCallbackType& callback);
      const WheelCallbackType& GetScrolledCallback() const;
      bool IsScrolledCallbackEnabled() const;
      void DisableScrolledCallback();

   protected:
      ~GenericMouseListener() {};

   private:
      ButtonCallbackType mPressedCB, mReleasedCB;
      ClickCallbackType mClickedCB;
      bool mPressedEnabled, mReleasedEnabled, mClickedEnabled;

      MovementCallbackType mMovedCB, mDraggedCB;
      bool mMovedEnabled, mDraggedEnabled;

      WheelCallbackType mScrolledCB;
      bool mScrolledEnabled;
   };
}

#endif  // DELTA_GENERIC_MOUSE_LISTENER
