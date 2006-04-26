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


#include <set>
#include <string>

#include <Producer/KeyboardMouse>
#include <dtCore/inputdevice.h>
#include <dtUtil/deprecationmgr.h>

namespace dtCore
{
   class DeltaWin;
   class MouseListener;

   /**
    * Mouse buttons.
    */
   enum MouseButton
   {
      LeftButton,
      MiddleButton,
      RightButton
   };

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
         /**
          * The list of mouse listeners.
          */
         typedef std::list<MouseListener*> MouseListenerList;

         /**
          * Gets the current mouse position.
          *
          * @param x a reference to the location in which to store the
          * x coordinate
          * @param y a reference to the location in which to store the
          * y coordinate
          */
         void GetPosition(float& x, float& y);
         
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
         bool GetButtonState(MouseButton button);

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
         virtual void mouseScroll( Producer::KeyboardMouseCallback::ScrollingMotion );
         ///Producer callback methods
         virtual void mouseMotion( float x, float y);
         ///Producer callback methods
         virtual void passiveMouseMotion( float x, float y);
         ///Producer callback methods
         virtual void buttonPress( float x, float y, unsigned int button );
         ///Producer callback methods
         virtual void doubleButtonPress( float x, float y , unsigned int button );
         ///Producer callback methods
         virtual void buttonRelease( float x, float y, unsigned int button);

         const MouseListenerList& GetListeners() const { return mMouseListeners; }

      protected:
         MouseListenerList mMouseListeners;

         Producer::KeyboardMouse* mKeyboardMouse;
   };


   /**
    * An interface for objects interested in mouse events.
    */
   class DT_CORE_EXPORT MouseListener
   {   
      public:

         virtual ~MouseListener() {}
      
         /**
          * Called when a button is pressed.
          *
          * @param mouse the source of the event
          * @param button the button pressed
          */
         virtual void ButtonPressed(Mouse* mouse, MouseButton button)
         {
            DEPRECATE("void ButtonPressed(Mouse* mouse, MouseButton button)",
                      "bool HandleButtonPressed(Mouse* mouse, MouseButton button)")
         }
      
         /**
          * Called when a button is pressed.
          *
          * @param mouse the source of the event
          * @param button the button pressed
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleButtonPressed(Mouse* mouse, MouseButton button);
      
         /**
          * Called when a button is released.
          *
          * @param mouse the source of the event
          * @param button the button released
          */
         virtual void ButtonReleased(Mouse* mouse, MouseButton button)
         {
            DEPRECATE("void ButtonReleased(Mouse* mouse, MouseButton button)",
                      "bool HandleButtonReleased(Mouse* mouse, MouseButton button)")
         }
      
         /**
          * Called when a button is released.
          *
          * @param mouse the source of the event
          * @param button the button released
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
         virtual bool HandleButtonReleased(Mouse* mouse, MouseButton button);
      
         /**
          * Called when a button is clicked.
          *
          * @param mouse the source of the event
          * @param button the button clicked
          * @param clickCount the click count
          */
         virtual void ButtonClicked(Mouse* mouse, MouseButton button, int clickCount)
         {
            DEPRECATE("void ButtonClicked(Mouse* mouse, MouseButton button, int clickCount)",
                      "bool HandleButtonClicked(Mouse* mouse, MouseButton button, int clickCount)")
         }
      
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
         virtual bool HandleButtonClicked(Mouse* mouse, MouseButton button, int clickCount);
            
         /**
          * Called when the mouse pointer is moved.
          *
          * @param mouse the source of the event
          * @param x the x coordinate
          * @param y the y coordinate
          */
         virtual void MouseMoved(Mouse* mouse, float x, float y)
         {
            DEPRECATE("void MouseMoved(Mouse* mouse, float x, float y)",
                      "bool HandleMouseMoved(Mouse* mouse, float x, float y)")
         }
      
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
         virtual bool HandleMouseMoved(Mouse* mouse, float x, float y);
            
         /**
          * Called when the mouse pointer is dragged.
          *
          * @param mouse the source of the event
          * @param x the x coordinate
          * @param y the y coordinate
          */
         virtual void MouseDragged(Mouse* mouse, float x, float y)
         {
            DEPRECATE("void MouseDragged(Mouse* mouse, float x, float y)",
                      "bool HandleMouseDragged(Mouse* mouse, float x, float y)")
         }
      
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
         virtual bool HandleMouseDragged(Mouse* mouse, float x, float y);
            
         /**
          * Called when the mouse is scrolled.
          *
          * @param mouse the source of the event
          * @param delta the scroll delta (+1 for up one, -1 for down one)
          */
         virtual void MouseScrolled(Mouse* mouse, int delta)
         {
            DEPRECATE("void MouseScrolled(Mouse* mouse, int delta)",
                      "bool HandleMouseScrolled(Mouse* mouse, int delta)")
         }

         /**
          * Called when the mouse is scrolled.
          *
          * @param mouse the source of the event
          * @param delta the scroll delta (+1 for up one, -1 for down one)
          * @return true if this MouseListener handled the event. The
          * Mouse calling this function is responsbile for using this
          * return value or not.
          */
        virtual bool HandleMouseScrolled(Mouse* mouse, int delta);
   };
}

#endif // DELTA_MOUSE
