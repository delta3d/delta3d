#include <prefix/dtcoreprefix.h>
#include <dtCore/generickeyboardlistener.h>

using namespace dtCore;

GenericKeyboardListener::GenericKeyboardListener() : BaseClass(),
   mPressedCB(),
   mReleasedCB(),
   mTypedCB(),
   mPressedEnabled(false),
   mReleasedEnabled(false),
   mTypedEnabled(false)
{
}

GenericKeyboardListener::~GenericKeyboardListener()
{
}

bool GenericKeyboardListener::HandleKeyPressed(const dtCore::Keyboard* kb, int kc)
{
   if( mPressedEnabled )
   {
      return mPressedCB(kb,kc);
   }

   return false;
}

bool GenericKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* kb, int kc)
{
   if( mReleasedEnabled )
   {
      return mReleasedCB(kb,kc);
   }

   return false;
}

bool GenericKeyboardListener::HandleKeyTyped(const dtCore::Keyboard* kb, int kc)
{
   if( mTypedEnabled )
   {
      return mTypedCB(kb,kc);
   }

   return false;
}

void GenericKeyboardListener::SetPressedCallback(const CallbackType& callback)
{
   mPressedCB = callback;
   mPressedEnabled = true;
}

const GenericKeyboardListener::CallbackType& GenericKeyboardListener::GetPressedCallback() const
{
   return mPressedCB;
}

bool GenericKeyboardListener::IsPressedCallbackEnabled() const
{
   return mPressedEnabled;
}

void GenericKeyboardListener::DisablePressedCallback()
{
   mPressedEnabled = false;
}

void GenericKeyboardListener::SetReleasedCallback(const CallbackType& callback)
{
   mReleasedCB = callback;
   mReleasedEnabled = true;
}

const GenericKeyboardListener::CallbackType& GenericKeyboardListener::GetReleasedCallback() const
{
   return mReleasedCB;
}

bool GenericKeyboardListener::IsReleasedCallbackEnabled() const
{
   return mReleasedEnabled;
}

void GenericKeyboardListener::DisableReleasedCallback()
{
   mReleasedEnabled = false;
}

void GenericKeyboardListener::SetTypedCallback(const CallbackType& callback)
{
   mTypedCB = callback;
   mTypedEnabled = true;
}

const GenericKeyboardListener::CallbackType& GenericKeyboardListener::GetTypedCallback() const
{
   return mTypedCB;
}

bool GenericKeyboardListener::IsTypedCallbackEnabled() const
{
   return mTypedEnabled;
}

void GenericKeyboardListener::DisableTypedCallback()
{
   mTypedEnabled = false;
}
