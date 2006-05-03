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
*/

#ifndef DELTA_MOUSE
#define DELTA_MOUSE

// mouse.h: Declaration of the Mouse class.
//
//////////////////////////////////////////////////////////////////////


#include <list>
#include <string>

#include <Producer/KeyboardMouse>
#include <dtCore/inputdevice.h>
#include <dtUtil/deprecationmgr.h>
#include <osg/Referenced>               // for listener's base class
#include <dtCore/refptr.h>             // for typedef, list member

namespace dtCore
{
   class DeltaWin;
   class MouseListener;

   /**
    * A mouse device.
    */
   class DT_CORE_EXPORT Mouse : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Mouse)

      friend class DeltaWin;

   protected:
      /**
      * Constructor.
      *
      * @param name the instance name
      */
      Mouse(Producer::KeyboardMouse* km,const std::string& name = "mouse");

      /**
      * Destructor.
      */
      virtual ~Mouse();

      public:
         /// Mouse buttons.
         enum MouseButton
         {
            LeftButton,
            MiddleButton,
            RightButton
         };

         /**
          * The list of mouse listeners.
          */
         typedef std::list<dtCore::RefPtr<MouseListener> > MouseListenerList;

         /**
          * Gets the current mouse position.
          *
          * @param x a reference to the location in which to store the
          * x coordinate
          * @param y a reference to the location in which to store the
          * y coordinate
          */
         void GetPosition(float& x, float& y) const;
         
         /**
         * Sets the current mouse position.
         *
         * @param x the new x coordinate
         * @param y the new y coordinate
         */
         void SetPosition(float x, float y);

         /**
          * Gets the state of the specified mouse button.
          *
          * @param button the button to check
          * @return true if the button is pressed, false otherwise
          */
         bool GetButtonState(MouseButton button) const;

         /**
          * Adds a listener for mouse events.
          *
          * @param mouseListener the listener to add
          */
         void AddMouseListener(MouseListener* mouseListener);

         /**
          * Removes a listener for mouse events.
          *
          * @param mouseListener the listener to remove
          */
         void RemoveMouseListener(MouseListener* mouseListener);

         ///Producer callback methods
         virtual bool MouseScroll( Producer::KeyboardMouseCallback::ScrollingMotion );
         ///Producer callback methods
         virtual bool MouseMotion( float x, float y);
         ///Producer callback methods
         virtual bool PassiveMouseMotion( float x, float y);
         ///Producer callback methods
         virtual bool ButtonPress( float x, float y, unsigned int button );
         ///Producer callback methods
         virtual bool DoubleButtonPress( float x, float y , unsigned int button );
         ///Producer callback methods
         virtual bool ButtonRelease( float x, float y, unsigned int button);

         const MouseListenerList& GetListeners() const { return mMouseListeners; }

      protected:
         MouseListenerList mMouseListeners;

         dtCore::RefPtr<Producer::KeyboardMouse> mKeyboardMouse;
   };


   /**
    * An interface for objects interested in mouse events.
    */
   class DT_CORE_EXPORT MouseListener : public osg::Referenced
   {   
      public:
         virtual ~MouseListener() {}

         /**
          * Called when a button is pressed.
          *
          * @param mouse the source of the event
          * @param button the button pressed
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleButtonPressed(const Mouse* mouse, Mouse::MouseButton button)=0;

         /**
          * Called when a button is released.
          *
          * @param mouse the source of the event
          * @param button the button released
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleButtonReleased(const Mouse* mouse, Mouse::MouseButton button)=0;

         /**
          * Called when a button is clicked.
          *
          * @param mouse the source of the event
          * @param button the button clicked
          * @param clickCount the click count
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleButtonClicked(const Mouse* mouse, Mouse::MouseButton button, int clickCount)=0;

         /**
          * Called when the mouse pointer is moved.
          *
          * @param mouse the source of the event
          * @param x the x coordinate
          * @param y the y coordinate
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleMouseMoved(const Mouse* mouse, float x, float y)=0;

         /**
          * Called when the mouse pointer is dragged.
          *
          * @param mouse the source of the event
          * @param x the x coordinate
          * @param y the y coordinate
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleMouseDragged(const Mouse* mouse, float x, float y)=0;

         /**
          * Called when the mouse is scrolled.
          *
          * @param mouse the source of the event
          * @param delta the scroll delta (+1 for up one, -1 for down one)
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
        virtual bool HandleMouseScrolled(const Mouse* mouse, int delta)=0;
   };
}

#endif // DELTA_MOUSE
