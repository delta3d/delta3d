// mouse.cpp: Implementation of the Mouse class.
//
//////////////////////////////////////////////////////////////////////

#include "mouse.h"
#include "notify.h"

using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Mouse)


/**
 * Constructor.
 *
 * @param name the instance name
 */
Mouse::Mouse(string name) : InputDevice(name)
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
 * @param x a pointer to the location in which to store the
 * x coordinate
 * @param y a pointer to the location in which to store the
 * y coordinate
 */
void Mouse::GetPosition(float* x, float* y)
{
   (*x) = GetAxis(0)->GetState();
   (*y) = GetAxis(1)->GetState();
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
   mouseListeners.insert(mouseListener);
}

/**
 * Removes a listener for mouse events.
 *
 * @param mouseListener the listener to remove
 */
void Mouse::RemoveMouseListener(MouseListener* mouseListener)
{
   mouseListeners.erase(mouseListener);
}

/**
 * Producer callback for mouse scroll events.
 *
 * @param sm the scroll type
 */
void Mouse::mouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm)
{
   int delta;

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

   for(set<MouseListener*>::iterator it = mouseListeners.begin();
       it != mouseListeners.end();
       it++)
   {
          (*it)->MouseScrolled(this, delta);
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

   for(set<MouseListener*>::iterator it = mouseListeners.begin();
       it != mouseListeners.end();
       it++)
   {
      (*it)->MouseDragged(this, x, y);
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

   for(set<MouseListener*>::iterator it = mouseListeners.begin();
       it != mouseListeners.end();
       it++)
   {
      (*it)->MouseMoved(this, x, y);
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
   MouseButton mouseButton = (MouseButton)(button-1);

   GetButton(mouseButton)->SetState(true);

   for(set<MouseListener*>::iterator it = mouseListeners.begin();
       it != mouseListeners.end();
       it++)
   {
      (*it)->ButtonPressed(this, mouseButton);
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
   MouseButton mouseButton = (MouseButton)(button-1);

   GetButton(mouseButton)->SetState(true);

   for(set<MouseListener*>::iterator it = mouseListeners.begin();
       it != mouseListeners.end();
       it++)
   {
      (*it)->ButtonClicked(this, mouseButton, 2);
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
   MouseButton mouseButton = (MouseButton)(button-1);

   GetButton(mouseButton)->SetState(false);

   for(set<MouseListener*>::iterator it = mouseListeners.begin();
       it != mouseListeners.end();
       it++)
   {
      (*it)->ButtonReleased(this, mouseButton);
   }
}
