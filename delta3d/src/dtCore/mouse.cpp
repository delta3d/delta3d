// mouse.cpp: Implementation of the Mouse class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>

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

   AddFeature(new Axis(this, "mouse x axis"));
   AddFeature(new Axis(this, "mouse y axis"));
   AddFeature(new Axis(this, "mouse z axis"));
   AddFeature(new Button(this, "left mouse button"));
   AddFeature(new Button(this, "middle mouse button"));
   AddFeature(new Button(this, "right mouse button"));
}

/**
 * Destructor.
 */
Mouse::~Mouse()
{
   DeregisterInstance(this);
}

/**
* Gets the current mouse position.
*
* @param x a reference to the location in which to store the
* x coordinate
* @param y a reference to the location in which to store the
* y coordinate
*/
void Mouse::GetPosition(float& x, float& y) const
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
   if( mKeyboardMouse.valid() )
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
bool Mouse::GetButtonState(MouseButton button) const
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
bool Mouse::MouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm)
{
   int delta(0);

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

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseScrolled(this, delta);
      ++iter;
   }

   return handled;
}

/**
 * Producer callback for mouse motion events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 */
bool Mouse::MouseMotion(float x, float y)
{
   GetAxis(0)->SetState(x, x - GetAxis(0)->GetState());
   GetAxis(1)->SetState(y, y - GetAxis(1)->GetState());

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseDragged(this, x, y);
      ++iter;
   }

   return handled;
}

/**
 * Producer callback for passive mouse motion events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 */
bool Mouse::PassiveMouseMotion(float x, float y)
{
   GetAxis(0)->SetState(x, x - GetAxis(0)->GetState());
   GetAxis(1)->SetState(y, y - GetAxis(1)->GetState());

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseMoved(this, x, y);
      ++iter;
   }

   return handled;
}

/**
 * Producer callback for button press events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param button the button identifier
 */
bool Mouse::ButtonDown(float x, float y, unsigned int button)
{
   if ((int)button > GetButtonCount() ) return false;

   MouseButton mouseButton = MouseButton(button-1);

   GetButton(mouseButton)->SetState(true);

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonPressed(this, mouseButton);
      ++iter;
   }

   return handled;
}

/**
 * Producer callback for double button press events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param button the button identifier
 */
bool Mouse::DoubleButtonDown(float x, float y, unsigned int button)
{
   if (int(button) > GetButtonCount() ) return false;

   MouseButton mouseButton = MouseButton(button-1);
   GetButton(mouseButton)->SetState(true);

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonClicked(this, mouseButton, 2);
      ++iter;
   }

   return handled;
}

/**
 * Producer callback for button release events.
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @param button the button identifier
 */
bool Mouse::ButtonUp(float x, float y, unsigned int button)
{
   if (int(button) > GetButtonCount() ) return false;

   MouseButton mouseButton = MouseButton(button-1);
   GetButton(mouseButton)->SetState(false);

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonReleased(this, mouseButton);
      ++iter;
   }

   return handled;
}
