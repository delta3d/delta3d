#ifndef DELTA_TESTWINAPP
#define DELTA_TESTWINAPP

#include <CEGUI/CEGUI.h>

#include <dtABC/application.h>
#include <dtGUI/ceuidrawable.h>
#include <dtCore/deltawin.h>

class TestWinApp : public dtABC::Application
{

public:
   TestWinApp( const std::string& configFilename = "config.xml" );
protected:
   ~TestWinApp() {};
public:

   virtual void Config();
   virtual void PostFrame( const double deltaFrameTime );

private:
   dtCore::RefPtr<dtGUI::CEUIDrawable> mGUI;
   dtCore::DeltaWin::ResolutionVec mResolutionVec; ///<vector of available resolutions

   void BuildGUI( dtCore::DeltaWin::ResolutionVec &resolutions,
                  dtCore::DeltaWin::Resolution &currentRes);

   void UpdateWidgets();

   static void FullScreenToggleCB(const CEGUI::EventArgs &e);
   static void WindowPositionCB(const CEGUI::EventArgs &e);
   static void WindowTitleCB( const CEGUI::EventArgs &e);
   static void ChangeResolutionCB( const CEGUI::EventArgs &e);
};

#endif // DELTA_TESTWINAPP
