#include <dtABC/applicationmouselistener.h>
#include <dtABC/baseabc.h>

using namespace dtABC;

ApplicationMouseListener::ApplicationMouseListener() : BaseClass(),
   mApplication(0)
{
}

ApplicationMouseListener::ApplicationMouseListener(dtABC::BaseABC* app) : BaseClass(),
   mApplication(app)
{
}

ApplicationMouseListener::~ApplicationMouseListener()
{
}

void ApplicationMouseListener::SetApplication(dtABC::BaseABC* app)
{
   mApplication = app;
}

bool ApplicationMouseListener::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   return false;
}

bool ApplicationMouseListener::HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   return false;
}

bool ApplicationMouseListener::HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
{
   return false;
}

bool ApplicationMouseListener::HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
{
   return false;
}

bool ApplicationMouseListener::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   return false;
}

bool ApplicationMouseListener::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
{
   return false;
}

