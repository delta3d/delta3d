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

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/datatype.h>

#include <dtDirectorQt/propertyeditor.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/dynamicactorcontrol.h>
#include <dtEditQt/dynamicgameeventcontrol.h>
#include <dtEditQt/dynamicgrouppropertycontrol.h>
#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/pluginmanager.h>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>

const std::string DirectorToolPlugin::PLUGIN_NAME = "Director Tool";


////////////////////////////////////////////////////////////////////////////////
DirectorToolPlugin::DirectorToolPlugin(MainWindow* mw)
   : dtDirector::DirectorEditor(NULL)
   , mMainWindow(mw)
   , mProxy(NULL)
   , mNode(NULL)
{
   // Register some custom property types.
   dtDirector::PropertyEditor* propEditor = GetPropertyEditor();
   if (propEditor)
   {
      dtQt::DynamicControlFactory& dcfactory = propEditor->GetDynamicControlFactory();

      size_t datatypeCount = dtDAL::DataType::EnumerateType().size();

      for (size_t i = 0; i < datatypeCount; ++i)
      {
         dtDAL::DataType* dt = dtDAL::DataType::EnumerateType()[i];
         if (dt->IsResource())
         {
            dcfactory.RegisterControlForDataType<DynamicResourceControl>(*dt);
         }
      }

      dcfactory.RegisterControlForDataType<DynamicActorControl>(dtDAL::DataType::ACTOR);
      dcfactory.RegisterControlForDataType<DynamicGameEventControl>(dtDAL::DataType::GAME_EVENT);
      dcfactory.RegisterControlForDataType<DynamicGroupPropertyControl>(dtDAL::DataType::GROUP);
   }

   // Add a Director editor button in the tool bar.
   mToolButton = new QAction(QIcon(":/icons/tool.png"), "Director Editor", this);
   mToolButton->setCheckable(true);

   QToolBar* editToolBar = mMainWindow->GetEditToolbar();
   if (editToolBar)
   {
      editToolBar->addSeparator();
      editToolBar->addAction(mToolButton);
   }

   connect(mToolButton, SIGNAL(changed()), this, SLOT(OnToolButtonPressed()));
   connect(&dtEditQt::EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
      this, SLOT(OnMapChanged()));

   dtDirector::Director* director = new dtDirector::Director();
   director->Init(NULL, dtEditQt::EditorData::GetInstance().getCurrentMap());

   SetDirector(director);
}

////////////////////////////////////////////////////////////////////////////////
DirectorToolPlugin::~DirectorToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
std::vector<dtDAL::BaseActorObject*> DirectorToolPlugin::GetActorSelection()
{
   std::vector<dtDAL::BaseActorObject*> selection;
   EditorData::GetInstance().GetSelectedActors(selection);

   return selection;
}

//////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::OnMapChanged()
{
   GetDirector()->SetMap(dtEditQt::EditorData::GetInstance().getCurrentMap());
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::OnToolButtonPressed()
{
   if (mToolButton->isChecked())
   {
      show();
   }
   else
   {
      hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::OnGotoActor()
{
   if (mProxy.valid())
   {
      std::vector<dtCore::RefPtr<dtDAL::BaseActorObject> > toSelect;
      toSelect.push_back(mProxy);

      EditorEvents::GetInstance().emitActorsSelected(toSelect);
      EditorEvents::GetInstance().emitGotoActor(mProxy);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::OnUseCurrentActor()
{
   if (mNode && mNode->GetType().GetFullName() == "General.Actor")
   {
      std::vector<dtDAL::BaseActorObject*> selection;
      EditorData::GetInstance().GetSelectedActors(selection);

      if (selection.size())
      {
         mProxy = selection[0];

         dtDAL::ActorIDActorProperty* prop = dynamic_cast<dtDAL::ActorIDActorProperty*>(mNode->GetProperty("Value"));
         if (prop)
         {
            prop->SetValue(mProxy->GetId());
            Refresh();
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DirectorToolPlugin::OnContextValueNode(dtCore::RefPtr<dtDirector::Node> node, QMenu& menu)
{
   mNode = node;
   if (node->GetType().GetFullName() == "General.Actor" || node->GetType().GetFullName() == "Core.Player")
   {
      dtDAL::ActorIDActorProperty* prop = dynamic_cast<dtDAL::ActorIDActorProperty*>(node->GetProperty("Value"));
      if (prop)
      {
         bool hasDefault = false;
         mProxy = prop->GetActorProxy();
         if (mProxy)
         {
            QAction* gotoActorAction = menu.addAction("Go to Referenced Actor");
            connect(gotoActorAction, SIGNAL(triggered()), this, SLOT(OnGotoActor()));
            menu.setDefaultAction(gotoActorAction);
            hasDefault = true;
         }

         QAction* useCurActorAction = menu.addAction("Use current Actor");
         connect(useCurActorAction, SIGNAL(triggered()), this, SLOT(OnUseCurrentActor()));

         menu.addSeparator();
         return hasDefault;
      }
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DirectorToolPlugin::OnDoubleClickValueNode(dtCore::RefPtr<dtDirector::Node> node)
{
   if (node->GetType().GetFullName() == "General.Actor" || node->GetType().GetFullName() == "Core.Player")
   {
      dtDAL::ActorIDActorProperty* prop = dynamic_cast<dtDAL::ActorIDActorProperty*>(node->GetProperty("Value"));
      if (prop)
      {
         mProxy = prop->GetActorProxy();
         if (mProxy)
         {
            OnGotoActor();
            return true;
         }
      }
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::Destroy()
{
   if (mToolButton) mToolButton->setChecked(false);
}

//////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::closeEvent(QCloseEvent* event)
{
   if (mToolButton) mToolButton->setChecked(false);
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
