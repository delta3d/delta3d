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


#include <list>                        // for typedef, member.
#include <string>                      // for parameter

#include <osg/Referenced>              // for listener's base class
#include <osg/observer_ptr>
#include <osg/Vec2>
#include <osgGA/GUIEventAdapter>


#include <dtCore/inputdevice.h>        // for base class
#include <dtCore/refptr.h>             // for typedef, list member
#include <dtCore/view.h>             // for observer_ptr<View>


namespace dtCore
{
   class MouseListener;
   
   /// The model of the mouse used throughout Delta3D.
   class DT_CORE_EXPORT Mouse : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Mouse)

   protected:

      /// Destructor.
      virtual ~Mouse();

   public:
      /// Constructor.
      /// @param name the instance name
      Mouse(const std::string& name = "mouse");
      
      /// Constructor.
      /// @param view owner of this instance
      /// @param name the instance name
      Mouse(dtCore::View * view, const std::string& name = "mouse");

      /// Mouse buttons.
      enum MouseButton
      {
         LeftButton,
         MiddleButton,
         RightButton
      };

      /// The list of mouse listeners.
      typedef std::list<dtCore::RefPtr<MouseListener> > MouseListenerList;

      /// Gets the current mouse position.
      /// @param x a reference to the location in which to store the x coordinate
      /// @param y a reference to the location in which to store the y coordinate
      void GetPosition(float& x, float& y) const;
      osg::Vec2 GetPosition() const;

      /// Sets the current mouse position.
      /// @param x the new x coordinate
      /// @param y the new y coordinate
      void SetPosition(float x, float y);
      void SetPosition(const osg::Vec2& xy);

      /// Gets the state of the specified mouse button.
      /// @param button the button to check
      /// @return true if the button is pressed, false otherwise
      bool GetButtonState(MouseButton button) const;

      /// Pushes a listener to the back of its list.
      /// @param mouseListener the listener to add
      void AddMouseListener(MouseListener* mouseListener);

      /// Inserts the listener into the list at a position BEFORE pos.
      void InsertMouseListener(const MouseListenerList::value_type& pos, MouseListener* ml);

      /// Removes a listener for mouse events.
      /// @param mouseListener the listener to remove
      void RemoveMouseListener(MouseListener* mouseListener);

      /// Tries to find the given the listener in our list of listeners
      /// @param mouseListener the listener to check
      /// @return true if the mouse listener is in our list
      bool HasMouseListener(MouseListener* mouseListener);

      /// For injecting mouse scroll events.
      /// @param sm the scroll type
      virtual bool MouseScroll(osgGA::GUIEventAdapter::ScrollingMotion sm);

      /// For injecting mouse drag events.
      /// @param x the x coordinate
      /// @param y the y coordinate
      virtual bool MouseMotion( float x, float y);

      /// For injecting passive mouse motion events.
      /// @param x the x coordinate
      /// @param y the y coordinate
      virtual bool PassiveMouseMotion( float x, float y);

      /// For injecting button press events.
      /// @param x the x coordinate
      /// @param y the y coordinate
      /// @param button the button identifier
      virtual bool ButtonDown( float x, float y, MouseButton button );

      /// For injecting double button press events.
      /// @param x the x coordinate
      /// @param y the y coordinate
      /// @param button the button identifier
      virtual bool DoubleButtonDown( float x, float y , MouseButton button );

      /// For injecting button release events.
      /// @param x the x coordinate
      /// @param y the y coordinate
      /// @param button the button identifier
      virtual bool ButtonUp( float x, float y, MouseButton button);

      /// @return the container of MouseListener instances.
      const MouseListenerList& GetListeners() const { return mMouseListeners; }

      ///@return if the mouse has window focus, false otherwise
      bool GetHasFocus();

      /// @return the mOsgViewerView
      dtCore::View * GetView() { return mView.get(); }
      
      /// @return the const mView
      const dtCore::View * GetView() const { return mView.get(); }
      
   protected:
      
      friend class KeyboardMouseHandler;
      /// define the mOsgViewerView
      void SetView(dtCore::View * view) { mView = view; }
      
      
      /// The container of observers.
      MouseListenerList mMouseListeners;
      
      /// needed to control the cursor on the window.
      osg::observer_ptr<dtCore::View> mView;
   };

   /// An interface for objects interested in mouse events.
   class DT_CORE_EXPORT MouseListener : public osg::Referenced
   {   
   protected:
      virtual ~MouseListener() {}
   public:

      /// Called when a button is pressed.
      /// @param mouse the source of the event
      /// @param button the button pressed
      /// @return true if this MouseListener handled the event. The
      /// Mouse calling this function is responsbile for using this
      /// return value or not.
      virtual bool HandleButtonPressed(const Mouse* mouse, Mouse::MouseButton button)=0;

      /// Called when a button is released.
      /// @param mouse the source of the event
      /// @param button the button released
      /// @return true if this MouseListener handled the event. The
      /// Mouse calling this function is responsbile for using this
      /// return value or not.
      virtual bool HandleButtonReleased(const Mouse* mouse, Mouse::MouseButton button)=0;

      /// Called when a button is clicked.
      /// @param mouse the source of the event
      /// @param button the button clicked
      /// @param clickCount the click count
      /// @return true if this MouseListener handled the event. The
      /// Mouse calling this function is responsbile for using this
      /// return value or not.
      virtual bool HandleButtonClicked(const Mouse* mouse, Mouse::MouseButton button, int clickCount)=0;

      /// Called when the mouse pointer is moved.
      /// @param mouse the source of the event
      /// @param x the x coordinate
      /// @param y the y coordinate
      /// @return true if this MouseListener handled the event. The
      /// Mouse calling this function is responsbile for using this
      /// return value or not.
      virtual bool HandleMouseMoved(const Mouse* mouse, float x, float y)=0;

      /// Called when the mouse pointer is dragged.
      /// @param mouse the source of the event
      /// @param x the x coordinate
      /// @param y the y coordinate
      /// @return true if this MouseListener handled the event. The
      /// Mouse calling this function is responsbile for using this
      /// return value or not.
      virtual bool HandleMouseDragged(const Mouse* mouse, float x, float y)=0;

      /// Called when the mouse is scrolled.
      /// @param mouse the source of the event
      /// @param delta the scroll delta (+1 for up one, -1 for down one)
      /// @return true if this MouseListener handled the event. The
      /// Mouse calling this function is responsbile for using this
      /// return value or not.
      virtual bool HandleMouseScrolled(const Mouse* mouse, int delta)=0;
   };
}

#endif // DELTA_MOUSE
