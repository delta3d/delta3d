/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
#include <prefix/unittestprefix.h>
//#include <prefix/dtgameprefix.h>
#include "guiapp.h"
#include "guimanager.h"
#include <dtGUI/scriptmodule.h>
#include <dtUtil/datapathutils.h>
#include <string>

using namespace dtTest;

GUIApp::GUIApp() : BaseClass(),
   mGUIManager(new dtGUI::GUIManager()),
   mScriptModule(new dtGUI::ScriptModule())
{
   // should only need this for delta .xsd files
   std::string deltadata(dtUtil::GetDeltaRootPath() + "/data");

   // need this for gui .xsd files
   std::string deltagui( "/gui" );

   ///\todo need to decide how paths will be handled.  We need to decide if DELTA_DATA is a list or a single item.
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ":" + deltadata + deltagui);
}

GUIApp::~GUIApp()
{
   mGUIManager->ShutdownGUI();

   delete mScriptModule;
}

void GUIApp::Config()
{
   BaseClass::Config();

   mGUIManager->InitializeGUI( GetWindow(), GetKeyboard(), GetMouse(), mScriptModule );
}

dtGUI::GUIManager* GUIApp::GetGUIManager()
{
   return mGUIManager.get();
}

dtGUI::ScriptModule* GUIApp::GetScriptModule()
{
   return mScriptModule;
}

