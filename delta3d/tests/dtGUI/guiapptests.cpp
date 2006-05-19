#include "guiapp.h"
#include "guimanager.h"
#include <dtGUI/scriptmodule.h>
#include <dtCore/globals.h>
#include <string>

using namespace dtTest;

GUIApp::GUIApp() : BaseClass(),
   mGUIManager(new dtGUI::GUIManager()),
   mScriptModule(new dtGUI::ScriptModule())
{
   // should only need this for delta .xsd files
   std::string deltadata( dtCore::GetDeltaRootPath() + "/data" );

   // need this for gui .xsd files
   std::string deltagui( "/gui" );

   ///\todo need to decide how paths will be handled.  We need to decide if DELTA_DATA is a list or a single item.
   dtCore::SetDataFilePathList( dtCore::GetDeltaDataPathList() + ":" + deltadata + deltagui );
}

GUIApp::~GUIApp()
{
   mGUIManager->ShutdownGUI();

   delete mScriptModule;
}

void GUIApp::Config()
{
   BaseClass::Config();

   mGUIManager->InitializeGUI( GetWindow(), mScriptModule );
}

dtGUI::GUIManager* GUIApp::GetGUIManager()
{
   return mGUIManager.get();
}

dtGUI::ScriptModule* GUIApp::GetScriptModule()
{
   return mScriptModule;
}

