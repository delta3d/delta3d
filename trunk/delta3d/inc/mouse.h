// mouse.h: Declaration of the Mouse class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_MOUSE
#define DELTA_MOUSE

#include <set>
#include <string>

#include "Producer/KeyboardMouse"

#include "inputdevice.h"


namespace dtCore
{
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
   class DT_EXPORT Mouse : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Mouse)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Mouse(std::string name = "mouse");

         /**
          * Destructor.
          */
         virtual ~Mouse();

         /**
          * Gets the current mouse position.
          *
          * @param x a pointer to the location in which to store the
          * x coordinate
          * @param y a pointer to the location in which to store the
          * y coordinate
          */
         void GetPosition(float* x, float* y);

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
         virtual void mouseMotion( float, float);
         virtual void passiveMouseMotion( float, float);
         virtual void buttonPress( float, float, unsigned int );
         virtual void doubleButtonPress( float, float, unsigned int );
         virtual void buttonRelease( float, float, unsigned int );


      private:

         /**
          * The list of mouse listeners.
          */
         std::set<MouseListener*> mouseListeners;
   };


   /**
    * An interface for objects interested in mouse events.
    */
   class DT_EXPORT MouseListener
   {
      public:

         /**
          * Called when a button is pressed.
          *
          * @param mouse the source of the event
          * @param button the button pressed
          */
         virtual void ButtonPressed(Mouse* mouse, MouseButton button) {}

         /**
          * Called when a button is released.
          *
          * @param mouse the source of the event
          * @param button the button released
          */
         virtual void ButtonReleased(Mouse* mouse, MouseButton button) {}

         /**
          * Called when a button is clicked.
          *
          * @param mouse the source of the event
          * @param button the button clicked
          * @param clickCount the click count
          */
         virtual void ButtonClicked(Mouse* mouse, MouseButton button, int clickCount) {}

         /**
          * Called when the mouse pointer is moved.
          *
          * @param mouse the source of the event
          * @param x the x coordinate
          * @param y the y coordinate
          */
         virtual void MouseMoved(Mouse* mouse, float x, float y) {}

         /**
          * Called when the mouse pointer is dragged.
          *
          * @param mouse the source of the event
          * @param x the x coordinate
          * @param y the y coordinate
          */
         virtual void MouseDragged(Mouse* mouse, float x, float y) {}

         /**
          * Called when the mouse is scrolled.
          *
          * @param mouse the source of the event
          * @param delta the scroll delta (+1 for up one, -1 for down one)
          */
         virtual void MouseScrolled(Mouse* mouse, int delta) {}
   };
};

#endif // DELTA_MOUSE
