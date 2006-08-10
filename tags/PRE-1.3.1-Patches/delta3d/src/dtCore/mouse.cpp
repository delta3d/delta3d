// mouse.cpp: Implementation of the Mouse class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <algorithm>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Mouse)

 Mouse::Mouse(Producer::KeyboardMouse* km, const std::string& name) : InputDevice(name), mKeyboardMouse(km)
{
   RegisterInstance(this);

   AddFeature(new Axis(this, "mouse x axis"));
   AddFeature(new Axis(this, "mouse y axis"));
   AddFeature(new Axis(this, "mouse scroll axis"));
   AddFeature(new Button(this, "left mouse button"));
   AddFeature(new Button(this, "middle mouse button"));
   AddFeature(new Button(this, "right mouse button"));
}

Mouse::~Mouse()
{
   DeregisterInstance(this);
}

void Mouse::GetPosition(float& x, float& y) const
{
   x = GetAxis(0)->GetState();
   y = GetAxis(1)->GetState();
}

void Mouse::SetPosition(float x, float y)
{
   if( mKeyboardMouse.valid() )
   {
      mKeyboardMouse->positionPointer(x,y);
   }
}

bool Mouse::GetButtonState(MouseButton button) const
{
   return GetButton(button)->GetState();
}

void Mouse::AddMouseListener(MouseListener* mouseListener)
{
   mMouseListeners.push_back(mouseListener);
}

void Mouse::InsertMouseListener(const MouseListenerList::value_type& pos, MouseListener* ml)
{
   MouseListenerList::iterator iter = std::find( mMouseListeners.begin() , mMouseListeners.end() , pos );
   mMouseListeners.insert(iter,ml);
}

void Mouse::RemoveMouseListener(MouseListener* mouseListener)
{
   mMouseListeners.remove(mouseListener);
}

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

   // a workaround for classes that listen to input device features
   dtCore::RefPtr<Axis> scrollAxis = GetAxis(2);
   if( !handled )
   {
      handled = scrollAxis->SetState( scrollAxis->GetState() + delta, delta );
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      scrollAxis->SetState( scrollAxis->GetState() + delta, delta );
   }

   return handled;
}

bool Mouse::MouseMotion(float x, float y)
{
   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseDragged(this, x, y);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   Axis* zero = GetAxis(0);
   Axis* one = GetAxis(1);
   if( !handled )
   {
      bool zero_handled = zero->SetState(x, x - zero->GetState());
      bool one_handled = one->SetState(y, y - one->GetState());
      handled = (one_handled || zero_handled);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      zero->SetState(x, x - zero->GetState());
      one->SetState(y, y - one->GetState());
   }

   return handled;
}

bool Mouse::PassiveMouseMotion(float x, float y)
{
   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseMoved(this, x, y);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   Axis* zero = GetAxis(0);
   Axis* one = GetAxis(1);
   if( !handled )  // affect the return value
   {
      bool zero_handled = zero->SetState(x, x - zero->GetState());
      bool one_handled = one->SetState(y, y - one->GetState());
      handled = (one_handled || zero_handled);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      zero->SetState(x, x - zero->GetState());
      one->SetState(y, y - one->GetState());
   }

   return handled;
}

bool Mouse::ButtonDown(float x, float y, MouseButton button)
{
   if (int(button) > GetButtonCount() ) return false;

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonPressed(this, button);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   if( !handled )  // affect the return value
   {
      handled = GetButton(button)->SetState(true);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(button)->SetState(true);
   }

   return handled;
}

bool Mouse::DoubleButtonDown(float x, float y, MouseButton button)
{
   if (int(button) > GetButtonCount() ) return false;

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonClicked(this, button, 2);
      ++iter;
   }

   if( !handled )  // affect the return value
   {
      handled = GetButton(button)->SetState(true);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(button)->SetState(true);
   }

   return handled;
}

bool Mouse::ButtonUp(float x, float y, MouseButton button)
{
   if (int(button) > GetButtonCount() ) return false;

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonReleased(this, button);
      ++iter;
   }

   if( !handled )   // affect the return value
   {
      handled = GetButton(button)->SetState(false);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(button)->SetState(false);
   }

   return handled;
}
