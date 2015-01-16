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

#include "testwin.h"

#include <dtGUI/scriptmodule.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>
#include <dtCore/scene.h>

#include <sstream>

using namespace dtCore;
using namespace dtABC;
using namespace dtGUI;
using namespace dtUtil;

TestWinApp::TestWinApp(const std::string& configFilename)
: Application(configFilename)
, mGUI(NULL)
, mResolutionVec()
, mScriptModule(new dtGUI::ScriptModule())
{
}

TestWinApp::~TestWinApp()
{
   delete mScriptModule;
}

void TestWinApp::Config()
{
   dtABC::Application::Config();

   GetWindow()->SetShowCursor(false);

   mGUI = new dtGUI::GUI(GetCamera(),
                         GetKeyboard(),
                         GetMouse());

   CEGUI::System::getSingleton().setScriptingModule(mScriptModule);

   mScriptModule->AddCallback("FullScreenToggleCB",   CEGUI::SubscriberSlot(&TestWinApp::FullScreenToggleCB, this));
   mScriptModule->AddCallback("WindowPositionCB", CEGUI::SubscriberSlot(&TestWinApp::WindowPositionCB, this));
   mScriptModule->AddCallback("WindowTitleCB", CEGUI::SubscriberSlot(&TestWinApp::WindowTitleCB, this));
   mScriptModule->AddCallback("ChangeResolutionCB", CEGUI::SubscriberSlot(&TestWinApp::ChangeResolutionCB, this));

   // dump all valid resolutions into a vector
   mResolutionVec = DeltaWin::GetResolutions();
   DeltaWin::Resolution currRes = DeltaWin::GetCurrentResolution();

   //create the GUI and fill it out
   BuildGUI(mResolutionVec, currRes);
}

void TestWinApp::PostFrame(const double deltaFrameTime)
{
   UpdateWidgets();
}

/** Setup the GUI with starting values.
*/
void TestWinApp::BuildGUI(DeltaWin::ResolutionVec& resolutions,
                          DeltaWin::Resolution& currentRes)
{
   try
   {
      mGUI->LoadScheme("WindowsLook.scheme");

      //overwrite the default search path, to found our local layout file.
      mGUI->SetResourceGroupDirectory("layouts", dtUtil::GetDeltaRootPath() + "/examples/testWin/");
      mGUI->LoadLayout("testWinLayout.xml");
      CEGUI::MultiColumnList* list = static_cast<CEGUI::MultiColumnList*>(mGUI->GetWidget("List"));

      int rowNum = 0;
      std::string str;

      for (DeltaWin::ResolutionVec::iterator itr = resolutions.begin();
           itr != resolutions.end();
           itr++)
      {
         rowNum = list->addRow();

         str = ToString<int>((*itr).width);
         CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(str);
         item->setTextColours(CEGUI::colour(0.0f, 0.0f, 0.0f));
         item->setSelectionColours(CEGUI::colour(1.0f, 0.0f, 0.0f));
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData(static_cast<void*>(&(*itr)));
         list->setItem(item, 0, rowNum);

         str = ToString<int>((*itr).height);
         item = new CEGUI::ListboxTextItem(str);
         item->setTextColours(CEGUI::colour(0.0f, 0.0f, 0.0f));
         item->setSelectionColours(CEGUI::colour(1.0f, 0.0f, 0.0f));
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData(static_cast<void*>(&(*itr)));
         list->setItem(item, 1, rowNum);

         str = ToString<int>((*itr).bitDepth);
         item = new CEGUI::ListboxTextItem(str);
         item->setTextColours(CEGUI::colour(0.0f, 0.0f, 0.0f));
         item->setSelectionColours(CEGUI::colour(1.0f, 0.0f, 0.0f));
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData(static_cast<void*>(&(*itr)));
         list->setItem(item, 2, rowNum);

         str = ToString<int>((*itr).refresh);
         item = new CEGUI::ListboxTextItem(str);
         item->setTextColours(CEGUI::colour(0.0f, 0.0f, 0.0f));
         item->setSelectionColours(CEGUI::colour(1.0f, 0.0f, 0.0f));
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData(static_cast<void*>(&(*itr)));
         list->setItem(item, 3, rowNum);
      }

      //use our current resolution to look up it's position in the available
      //resolution list.
      //Also select the GUI row which so happens to
      //correspond to the same index number
      int idx = DeltaWin::IsValidResolution(resolutions,
                                            currentRes.width, currentRes.height,
                                            currentRes.refresh, currentRes.bitDepth);
      if (idx > -1)
      {
         //todo also need to scroll the window to the correct row.
         //list->ensureItemIsVisible();
         list->setItemSelectState(CEGUI::MCLGridRef(idx, 0), true);
      }

      CEGUI::Checkbox* check = static_cast<CEGUI::Checkbox*>(mGUI->GetWidget("FullscreenToggle"));
      check->setSelected( GetWindow()->GetFullScreenMode());

      CEGUI::Editbox* title = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowTitle"));
      title->setText(GetWindow()->GetWindowTitle());

   }
   // catch to prevent exit (errors will be logged).
   catch (CEGUI::Exception &e)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,  "CEGUI::%s", e.getMessage().c_str());
   }
}

///called every frame
void TestWinApp::UpdateWidgets()
{
   static int lastX, lastY, lastW, lastH;

   int x, y, w, h;
   GetWindow()->GetPosition(x, y, w, h);

   if (lastX != x || lastY != y || lastW != w || lastH != h)
   {
      // if something is different, update the widgets
      lastX = x; lastY =y; lastW = w; lastH = h;

      CEGUI::Editbox* wBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowWidth"));
      wBox->setText(ToString(w).c_str());
      CEGUI::Editbox* hBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowHeight"));
      hBox->setText(ToString(h).c_str());
      CEGUI::Editbox* xBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowPosX"));
      xBox->setText(ToString(x).c_str());
      CEGUI::Editbox* yBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowPosY"));
      yBox->setText(ToString(y).c_str());
   }
}

bool TestWinApp::FullScreenToggleCB(const CEGUI::EventArgs& e)
{
   CEGUI::Checkbox* check = static_cast<CEGUI::Checkbox*>(mGUI->GetWidget("FullscreenToggle"));
   GetWindow()->SetFullScreenMode(check->isSelected());
   return true;
}

bool TestWinApp::WindowPositionCB(const CEGUI::EventArgs& e)
{
   int x, y, w, h;

   CEGUI::Editbox* wBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowWidth"));
   w = atoi(wBox->getText().c_str());

   CEGUI::Editbox* hBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowHeight"));
   h = atoi(hBox->getText().c_str());

   CEGUI::Editbox* xBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowPosX"));
   x = atoi(xBox->getText().c_str());

   CEGUI::Editbox* yBox = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowPosY"));
   y = atoi(yBox->getText().c_str());

   GetWindow()->SetPosition(x, y, w, h);
   return true;
}

bool TestWinApp::WindowTitleCB( const CEGUI::EventArgs& e)
{
   CEGUI::Editbox* box = static_cast<CEGUI::Editbox*>(mGUI->GetWidget("WindowTitle"));
   GetWindow()->SetWindowTitle(box->getText().c_str());
   return true;
}

bool TestWinApp::ChangeResolutionCB(const CEGUI::EventArgs& e)
{
   CEGUI::MultiColumnList* list = static_cast<CEGUI::MultiColumnList*>(mGUI->GetWidget("List"));

   CEGUI::ListboxItem* item = list->getFirstSelectedItem();

   DeltaWin::Resolution* res = static_cast<DeltaWin::Resolution*>(item->getUserData());
   DeltaWin::ChangeScreenResolution(*res);
   return true;
}


int main(int argc, char** argv)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(
      dtUtil::GetDeltaRootPath() + "/examples/data/;" +
      dtUtil::GetDeltaRootPath() + "/examples/data/gui/;" +
      dtUtil::GetDeltaRootPath() + "/examples/testWin/;" +
      dataPath + ";");

   RefPtr<TestWinApp> app = new TestWinApp("config.xml");

   app->Config();
   app->Run();

   return 0;
}
