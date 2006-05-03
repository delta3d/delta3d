#include <dtABC/applicationkeyboardlistener.h>
#include <dtABC/baseabc.h>

#include <dtCore/deltawin.h>   ///\todo needs to be included because of some issue in BaseABC.
#include <dtCore/mouse.h>      ///\todo needs to be included because of some issue in BaseABC.
#include <dtABC/applicationmouselistener.h>   ///\todo needs to be included because of some issue in BaseABC.
#include <dtCore/camera.h>   ///\todo needs to be included because of some issue in BaseABC.
#include <dtCore/scene.h>   ///\todo needs to be included because of some issue in BaseABC.

using namespace dtABC;

ApplicationKeyboardListener::ApplicationKeyboardListener() : BaseClass(),
   mApplication(0)
{
}

ApplicationKeyboardListener::ApplicationKeyboardListener(dtABC::BaseABC* app): BaseClass(),
   mApplication(app)
{
}

ApplicationKeyboardListener::~ApplicationKeyboardListener()
{
}

void ApplicationKeyboardListener::SetApplication(dtABC::BaseABC* app)
{
   mApplication = app;
}

bool ApplicationKeyboardListener::HandleKeyPressed(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
{
   return mApplication->KeyPressed(kb,key,kc);
}

bool ApplicationKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
{
   return mApplication->KeyReleased(kb,key,kc);
}

bool ApplicationKeyboardListener::HandleKeyTyped(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
{
   return false;
}


