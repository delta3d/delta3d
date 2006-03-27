// mouse.cpp: Implementation of the Mouse class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/mouse.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Mouse)

/**
 * Constructor.
 *
 * @param name the instance name
 */
 Mouse::Mouse(Producer::KeyboardMouse* km, const std::string& name) : InputDevice(name), mKeyboardMouse(km)
{
   RegisterInstance(this);

   AddFeature(
      new Axis(this, "mouse x axis")
   );

   AddFeature(
      new Axis(this, "mouse y axis")
   );

   AddFeature(
      new Axis(this, "mouse z axis")
   );

   AddFeature(
      new Button(this, "left mouse button")   
   );

   AddFeature(
      new Button(this, "middle mouse button")   
   );

   AddFeature(
      new Button(this, "right mouse button")   
   );
}

/**
 * Destructor.
 */
Mouse::~Mouse()
{
   DeregisterInstance(this);

   for(int i=0;i<GetFeatureCount();i++)
   {
      delete GetFeature(i);
   }
}

/**
* Gets the current mouse position.
*
* @param x a reference to the location in which to store the
* x coordinate
* @param y a reference to the location in which to store the
* y coordinate
*/
void Mouse::GetPosition(float& x, float& y)
{
   x = GetAxis(0)->GetState();
   y = GetAxis(1)->GetState();
}

/**
* Sets the current mouse position.
*
* @param x the new x coordinate
* @param y the new y coordinate
*/
void Mouse::SetPosition(float x, float y)
{
   if(mKeyboardMouse)
   {
      mKeyboardMouse->positionPointer(x,y);
   }
}

/**
 * Gets the state of the specified mouse button.
 *
 * @param button the button to check
 * @return true if the button is pressed, false otherwise
 */
bool Mouse::GetButtonState(MouseButton button)
{
   return GetButton(button)->GetState();
}

/**
 * Adds a listener for mouse events.
 *
 * @param mouseListener the listener to add
 */
void Mouse::AddMouseListener(MouseListener* mouseListener)
{
   mMouseListeners.push_back(mouseListener);
}

/**
 * Removes a listener for mouse events.
 *
 * @param mouseListener the listener to remove
 */
void Mouse::RemoveMouseListener(MouseListener* mouseListener)
{
   mMouseListeners.remove(mouseListener);
}

/**
 * Producer callback for mouse scroll events.
 *
 * @param sm the scroll type
 */
void Mouse::mouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm)
{
   int delta = 0;

   switch(sm)
   {
      case Producer::KeyboardMouseCallback::ScrollNone:
         delta = 0;
         break;

      case Producer::KeyboardMouseCallback::ScrollUp:
         delta = +1;
         break;

      case Producer::KeyboardMouseCallback::ScrollDown:
         delta = -1;
         break;
   }

   for(MouseListenerList::iterator it = mMouseListeners.begin();
       it != mMouseListeners.end();
       it++)
   {
          (*it)->HandleMouseScrolled(this, delta);
   }
}

/**
 * Producer callback for mouse motion events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 */
void Mouse::mouseMotion(float x, float y)
{
   GetAxis(0)->SetState(x, x - GetAxis(0)->GetState());
   GetAxis(1)->SetState(y, y - GetAxis(1)->GetState());

   for(MouseListenerList::iterator it = mMouseListeners.begin();
       it != mMouseListeners.end();
       it++)
   {
      (*it)->HandleMouseDragged(this, x, y);      
   }
}

/**
 * Producer callback for passive mouse motion events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 */
void Mouse::passiveMouseMotion(float x, float y)
{
   GetAxis(0)->SetState(x, x - GetAxis(0)->GetState());
   GetAxis(1)->SetState(y, y - GetAxis(1)->GetState());

   for(MouseListenerList::iterator it = mMouseListeners.begin();
       it != mMouseListeners.end();
       it++)
   {
      (*it)->HandleMouseMoved(this, x, y);      
   }
}

/**
 * Producer callback for button press events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param button the button identifier
 */
void Mouse::buttonPress(float x, float y, unsigned int button)
{
   if ((int)button > GetButtonCount() ) return;

   MouseButton mouseButton = MouseButton(button-1);

   GetButton(mouseButton)->SetState(true);

   for(MouseListenerList::iterator it = mMouseListeners.begin();
       it != mMouseListeners.end();
       it++)
   {
      (*it)->HandleButtonPressed(this, mouseButton);
   }
}

/**
 * Producer callback for double button press events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param button the button identifier
 */
void Mouse::doubleButtonPress(float x, float y, unsigned int button)
{
   if (int(button) > GetButtonCount() ) return;

   MouseButton mouseButton = MouseButton(button-1);

   GetButton(mouseButton)->SetState(true);

   for(MouseListenerList::iterator it = mMouseListeners.begin();
       it != mMouseListeners.end();
       it++)
   {
      (*it)->HandleButtonClicked(this, mouseButton, 2);
   }
}

/**
 * Producer callback for button release events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param button the button identifier
 */
void Mouse::buttonRelease(float x, float y, unsigned int button)
{
   if (int(button) > GetButtonCount() ) return;

   MouseButton mouseButton = MouseButton(button-1);

   GetButton(mouseButton)->SetState(false);

   for(MouseListenerList::iterator it = mMouseListeners.begin();
       it != mMouseListeners.end();
       it++)
   {
      (*it)->HandleButtonReleased(this, mouseButton);
   }
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
bool MouseListener::HandleButtonPressed( Mouse* mouse, MouseButton button )
{
   ButtonPressed( mouse, button );
   return true;
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
bool MouseListener::HandleButtonReleased( Mouse* mouse, MouseButton button )
{
   ButtonReleased( mouse, button );
   return true;
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
bool MouseListener::HandleButtonClicked( Mouse* mouse, MouseButton button, int clickCount )
{
   ButtonClicked( mouse, button, clickCount );
   return true;
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
bool MouseListener::HandleMouseMoved( Mouse* mouse, float x, float y )
{
   MouseMoved( mouse, x, y );
   return true;
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
bool MouseListener::HandleMouseDragged( Mouse* mouse, float x, float y )
{
   MouseDragged( mouse, x, y );
   return true;
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
bool MouseListener::HandleMouseScrolled( Mouse* mouse, int delta )
{
   MouseScrolled( mouse, delta );
   return true;
}
