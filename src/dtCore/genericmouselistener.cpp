#include <prefix/dtcoreprefix.h>
#include <dtCore/genericmouselistener.h>

using namespace dtCore;

//////////////////////////////////////////////////////////////////////////
GenericMouseListener::GenericMouseListener() : BaseClass(),
   mPressedCB(),
   mReleasedCB(),
   mClickedCB(),
   mPressedEnabled(false),
   mReleasedEnabled(false),
   mClickedEnabled(false),
   mMovedCB(),
   mDraggedCB(),
   mMovedEnabled(false),
   mDraggedEnabled(false),
   mScrolledCB(),
   mScrolledEnabled(false)
{
}

// inherited stuff
//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   if (mPressedEnabled)
   {
      return mPressedCB(mouse,button);
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   if (mReleasedEnabled)
   {
      return mReleasedCB(mouse,button);
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
{
   if (mClickedEnabled)
   {
      return mClickedCB(mouse,button,clickCount);
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
{
   if (mMovedEnabled)
   {
      return mMovedCB(mouse,x,y);
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   if (mDraggedEnabled)
   {
      return mDraggedCB(mouse,x,y);
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
{
   if (mScrolledEnabled)
   {
      return mScrolledCB(mouse,delta);
   }

   return false;
}

// pressed stuff
//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::SetPressedCallback(const ButtonCallbackType& callback)
{
   mPressedCB = callback;
   mPressedEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
const GenericMouseListener::ButtonCallbackType& GenericMouseListener::GetPressedCallback() const
{
   return mPressedCB;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::IsPressedCallbackEnabled() const
{
   return mPressedEnabled;
}

//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::DisablePressedCallback()
{
   mPressedEnabled = false;
}

// -- release stuff -- //
//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::SetReleasedCallback(const ButtonCallbackType& callback)
{
   mReleasedCB = callback;
   mReleasedEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
const GenericMouseListener::ButtonCallbackType& GenericMouseListener::GetReleasedCallback() const
{
   return mReleasedCB;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::IsReleasedCallbackEnabled() const
{
   return mReleasedEnabled;
}

//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::DisableReleasedCallback()
{
   mReleasedEnabled = false;
}

// -- click stuff -- //
//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::SetClickedCallback(const ClickCallbackType& callback)
{
   mClickedCB = callback;
   mClickedEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
const GenericMouseListener::ClickCallbackType& GenericMouseListener::GetClickedCallback() const
{
   return mClickedCB;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::IsClickedCallbackEnabled() const
{
   return mClickedEnabled;
}

//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::DisableClickedCallback()
{
   mClickedEnabled = false;
}

// moved stuff
//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::SetMovedCallback(const MovementCallbackType& callback)
{
   mMovedCB = callback;
   mMovedEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
const GenericMouseListener::MovementCallbackType& GenericMouseListener::GetMovedCallback() const
{
   return mMovedCB;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::IsMovedCallbackEnabled() const
{
   return mMovedEnabled;
}

//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::DisableMovedCallback()
{
   mMovedEnabled = false;
}

// dragged stuff
//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::SetDraggedCallback(const MovementCallbackType& callback)
{
   mDraggedCB = callback;
   mDraggedEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
const GenericMouseListener::MovementCallbackType& GenericMouseListener::GetDraggedCallback() const
{
   return mDraggedCB;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::IsDraggedCallbackEnabled() const
{
   return mDraggedEnabled;
}

//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::DisableDraggedCallback()
{
   mDraggedEnabled = false;
}

// scrolled stuff
//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::SetScrolledCallback(const WheelCallbackType& callback)
{
   mScrolledCB = callback;
   mScrolledEnabled = true;
}

//////////////////////////////////////////////////////////////////////////
const GenericMouseListener::WheelCallbackType& GenericMouseListener::GetScrolledCallback() const
{
   return mScrolledCB;
}

//////////////////////////////////////////////////////////////////////////
bool GenericMouseListener::IsScrolledCallbackEnabled() const
{
   return mScrolledEnabled;
}

//////////////////////////////////////////////////////////////////////////
void GenericMouseListener::DisableScrolledCallback()
{
   mScrolledEnabled = false;
}
