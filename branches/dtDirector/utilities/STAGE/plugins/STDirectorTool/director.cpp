/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Jeff P. Houde
 */

#include "director.h"

#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/editordata.h>

#include <Qt/qaction.h>
#include <Qt/qtoolbar.h>


const std::string DirectorToolPlugin::PLUGIN_NAME = "Director Tool";


////////////////////////////////////////////////////////////////////////////////
DirectorToolPlugin::DirectorToolPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mDirector(new dtDirector::Director())
   , mEditor(NULL)
{
   // Add a Director editor button in the tool bar.
   mToolButton = new QAction(QIcon(":/icons/tool.png"), "Director Editor", this);
   mToolButton->setCheckable(true);

   QToolBar* editToolBar = mMainWindow->GetEditToolbar();
   if (editToolBar)
   {
      editToolBar->addSeparator();
      editToolBar->addAction(mToolButton);
   }

   connect(mToolButton, SIGNAL(changed()), this, SLOT(onToolButtonPressed()));

   mDirector = new dtDirector::Director();
   mDirector->Init();

   // HACK: For now, create a default script for viewing.
   mDirector->CreateDebugScript();
}

////////////////////////////////////////////////////////////////////////////////
DirectorToolPlugin::~DirectorToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::onToolButtonPressed()
{
   if (mToolButton->isChecked())
   {
      if (!mEditor)
      {
         mEditor = new dtDirector::DirectorEditor(mDirector, NULL);
      }

      mEditor->show();
   }
   else
   {
      mEditor->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::Destroy()
{
   if (mEditor) delete mEditor;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace DirectorTool
{
class DT_DIRECTOR_TOOL_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return DirectorToolPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Plugin to access the editing tool for a Director Script."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
   }

   /** construct the plugin and return a pointer to it */
   virtual Plugin* Create(MainWindow* mw) 
   {
      mPlugin = new DirectorToolPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   Plugin* mPlugin;
}; 
} //namespace DirectorToolPlugin

extern "C" DT_DIRECTOR_TOOL_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new DirectorTool::PluginFactory;
}
