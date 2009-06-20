/* -*-c++-*-
* testWin - testwin (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef DELTA_TESTWINAPP
#define DELTA_TESTWINAPP

#ifdef None
#undef None
#endif
#include <CEGUI/CEGUI.h>

#include <dtABC/application.h>
#include <dtGUI/ceuidrawable.h>
#include <dtCore/deltawin.h>

namespace dtGUI
{
   class ScriptModule;
}

class TestWinApp : public dtABC::Application
{

public:
   TestWinApp( const std::string& configFilename = "config.xml" );
protected:
   ~TestWinApp();
public:

   virtual void Config();
   virtual void PostFrame( const double deltaFrameTime );

private:
   dtCore::RefPtr<dtGUI::CEUIDrawable> mGUI;
   dtCore::DeltaWin::ResolutionVec mResolutionVec; ///<vector of available resolutions
   dtGUI::ScriptModule* mScriptModule;

   void BuildGUI( dtCore::DeltaWin::ResolutionVec &resolutions,
                  dtCore::DeltaWin::Resolution &currentRes);

   void UpdateWidgets();

   bool FullScreenToggleCB(const CEGUI::EventArgs &e);
   bool WindowPositionCB(const CEGUI::EventArgs &e);
   bool WindowTitleCB( const CEGUI::EventArgs &e);
   bool ChangeResolutionCB( const CEGUI::EventArgs &e);
};

#endif // DELTA_TESTWINAPP
