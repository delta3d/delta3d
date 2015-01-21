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
#include "directoruiplugin.h"

#include <dtCore/actoridactorproperty.h>
#include <dtCore/datatype.h>

#include <dtDirectorQt/propertyeditor.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/dynamicactorcontrol.h>
#include <dtEditQt/dynamicgrouppropertycontrol.h>
#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/groupuiregistry.h>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>

const std::string DirectorToolPlugin::PLUGIN_NAME = "Director Tool";

////////////////////////////////////////////////////////////////////////////////
DirectorToolPlugin::DirectorToolPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mToolButton(NULL)
{
   // Add a Director editor button in the tool bar.
   mToolButton = new QAction(QIcon(":/icons/tool.png"), "Director Editor", this);

   QToolBar* editToolBar = mMainWindow->GetEditToolbar();
   if (editToolBar)
   {
      editToolBar->addAction(mToolButton);
   }

   connect(mToolButton, SIGNAL(triggered()), this, SLOT(OnToolButtonPressed()));

   static DirectorUIPlugin plugin;
   EditorData::GetInstance().GetGroupUIRegistry().RegisterPlugin(plugin);
}

////////////////////////////////////////////////////////////////////////////////
DirectorToolPlugin::~DirectorToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::Destroy()
{
   QToolBar* editToolBar = mMainWindow->GetEditToolbar();
   if (editToolBar)
   {
      editToolBar->removeAction(mToolButton);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolPlugin::OnToolButtonPressed()
{
   dtCore::RefPtr<dtDirector::Director> director = new dtDirector::Director();
   if (director)
   {
      director->Init(NULL, dtEditQt::EditorData::GetInstance().getCurrentMap());

      DirectorToolEditor* editor = new DirectorToolEditor();
      if (editor)
      {
         editor->SetDirector(director);
         editor->show();
      }
   }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
DirectorToolEditor::DirectorToolEditor()
   : dtDirector::DirectorEditor(NULL)
   , mProxy(NULL)
   , mNode(NULL)
{
   // Register some custom property types.
   dtDirector::PropertyEditor* propEditor = GetPropertyEditor();
   if (propEditor)
   {
      dtQt::DynamicControlFactory& dcfactory = propEditor->GetDynamicControlFactory();

      size_t datatypeCount = dtCore::DataType::EnumerateType().size();

      for (size_t i = 0; i < datatypeCount; ++i)
      {
         dtCore::DataType* dt = dtCore::DataType::EnumerateType()[i];
         if (dt->IsResource())
         {
            dcfactory.RegisterControlForDataType<STAGEDynamicResourceControl>(*dt);
         }
      }

      dcfactory.RegisterControlForDataType<STAGEDynamicActorControl>(dtCore::DataType::ACTOR);
      dcfactory.RegisterControlForDataType<STAGEDynamicGroupPropertyControl>(dtCore::DataType::GROUP);
   }

   connect(&dtEditQt::EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
      this, SLOT(OnMapChanged()));
}

////////////////////////////////////////////////////////////////////////////////
DirectorToolEditor::~DirectorToolEditor()
{
}

////////////////////////////////////////////////////////////////////////////////
dtCore::ActorPtrVector DirectorToolEditor::GetActorSelection()
{
   dtCore::ActorPtrVector selection;
   EditorData::GetInstance().GetSelectedActors(selection);

   return selection;
}

//////////////////////////////////////////////////////////////////////////
void DirectorToolEditor::OnMapChanged()
{
   if (GetDirector())
   {
      GetDirector()->SetMap(dtEditQt::EditorData::GetInstance().getCurrentMap());
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolEditor::OnGotoActor()
{
   if (mProxy.valid())
   {
      dtCore::ActorRefPtrVector toSelect;
      toSelect.push_back(mProxy);

      EditorEvents::GetInstance().emitActorsSelected(toSelect);
      EditorEvents::GetInstance().emitGotoActor(mProxy);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorToolEditor::OnUseCurrentActor()
{
   if (mNode && mNode->GetType().GetFullName() == "General.Actor")
   {
      dtCore::ActorPtrVector selection;
      EditorData::GetInstance().GetSelectedActors(selection);

      if (selection.size())
      {
         mProxy = selection[0];

         dtCore::ActorIDActorProperty* prop = dynamic_cast<dtCore::ActorIDActorProperty*>(mNode->GetProperty("Value"));
         if (prop)
         {
            prop->SetValue(mProxy->GetId());
            Refresh();
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DirectorToolEditor::OnContextValueNode(dtCore::RefPtr<dtDirector::Node> node, QMenu& menu)
{
   mNode = node;
   if (node->GetType().GetFullName() == "General.Actor" || node->GetType().GetFullName() == "Core.Player")
   {
      dtCore::ActorIDActorProperty* prop = dynamic_cast<dtCore::ActorIDActorProperty*>(node->GetProperty("Value"));
      if (prop)
      {
         bool hasDefault = false;
         mProxy = prop->GetActor();
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
bool DirectorToolEditor::OnDoubleClickValueNode(dtCore::RefPtr<dtDirector::Node> node)
{
   if (node->GetType().GetFullName() == "General.Actor" || node->GetType().GetFullName() == "Core.Player")
   {
      dtCore::ActorIDActorProperty* prop = dynamic_cast<dtCore::ActorIDActorProperty*>(node->GetProperty("Value"));
      if (prop)
      {
         mProxy = prop->GetActor();
         if (mProxy)
         {
            OnGotoActor();
            return true;
         }
      }
   }
   return false;
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
