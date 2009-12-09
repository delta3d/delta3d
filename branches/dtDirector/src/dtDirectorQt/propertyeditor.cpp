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

#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/nodeitem.h>
#include <dtDirectorQt/macroitem.h>

#include <dtDirector/node.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////
   PropertyEditor::PropertyEditor(DirectorEditor* parent)
      : dtQt::BasePropertyEditor(parent)
      , mDirectorEditor(parent)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditor::~PropertyEditor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditor::GetGroupBoxLabelText(const QString& baseGroupBoxName)
   {
      std::vector<dtDAL::PropertyContainer*> selectedContainers;
      GetSelectedPropertyContainers(selectedContainers);

      if (selectedContainers.size() == 1)
      {
         return QString(GetContainerGroupName(selectedContainers[0]).c_str()) + " selected";
      }

      return BaseClass::GetGroupBoxLabelText("Multiple nodes selected");
   }

   //////////////////////////////////////////////////////////////////////////
   std::string PropertyEditor::GetContainerGroupName(dtDAL::PropertyContainer* propertyContainer)
   {
      EventNode* eventNode = dynamic_cast<EventNode*>(propertyContainer);
      if (eventNode)
      {
         return std::string("Event Node '") + eventNode->GetType().GetFullName().c_str() + "'";
      }

      ActionNode* actionNode = dynamic_cast<ActionNode*>(propertyContainer);
      if (actionNode)
      {
         return std::string("Action Node '") + actionNode->GetType().GetFullName().c_str() + "'";
      }

      ValueNode* valueNode = dynamic_cast<ValueNode*>(propertyContainer);
      if (valueNode)
      {
         return std::string("Value Node '") + valueNode->GetType().GetFullName().c_str() + "'";
      }

      DirectorGraphData* graph = dynamic_cast<DirectorGraphData*>(propertyContainer);
      if (graph)
      {
         return std::string("Macro '") + graph->GetName().c_str() + "'";
      }

      Director* director = dynamic_cast<Director*>(propertyContainer);
      if (director)
      {
         return std::string("Director '") + director->GetName().c_str() + "'";
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   //void PropertyEditor::buildDynamicControls(dtDAL::PropertyContainer& propCon, dtQt::DynamicGroupControl* parentControl)
   //{
   //   //dtQt::DynamicGroupControl* parent = GetRootControl();
   //   //if (parentControl != NULL)
   //   //{
   //   //   parent = parentControl;
   //   //}

   //   //std::vector<dtDAL::ActorProperty*> propList;
   //   //propCon.GetPropertyList(propList);

   //   ////ViewportManager::GetInstance().emitModifyPropList(propCon, propList);

   //   BaseClass::buildDynamicControls(propCon, propList, parentControl);
   //}

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::closeEvent(QCloseEvent* e)
   {
      //if(EditorActions::GetInstance().mActionWindowsPropertyEditor != NULL)
      //{
      //   EditorActions::GetInstance().mActionWindowsPropertyEditor->setChecked(false);
      //}
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyAboutToChangeFromControl(dtDAL::PropertyContainer& propCon, dtDAL::ActorProperty& prop,
            const std::string& oldValue, const std::string& newValue)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyChangedFromControl(dtDAL::PropertyContainer& propCon, dtDAL::ActorProperty& prop)
   {
      // Check if the container is a node.
      Node* node = dynamic_cast<Node*>(&propCon);
      if (node)
      {
         NodeItem* item = mScene->GetNodeItem(node->GetID());

         // Re-Draw the node.
         if (item)
         {
            item->Draw();
            item->ConnectLinks(true);
         }

         return;
      }

      // Check if the container is a graph.
      DirectorGraphData* graph = dynamic_cast<DirectorGraphData*>(&propCon);
      if (graph)
      {
         MacroItem* item = mScene->GetGraphItem(graph);

         // Re-Draw the node.
         if (item)
         {
            item->Draw();
            item->ConnectLinks(true);
         }

         // Update all tabs to make sure their names are correct.
         int count = mGraphTabs->count();
         for (int index = 0; index < count; index++)
         {
            EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
            if (view && view->GetScene()->GetGraph() == graph)
            {
               mGraphTabs->setTabText(index, graph->GetName().c_str());
            }
         }
         return;
      }

      // Check if the container is the director.
      Director* director = dynamic_cast<Director*>(&propCon);
      if (director)
      {
         UpdateTitle();

         mDirectorEditor->setWindowTitle(director->GetGraphData()->GetName().c_str());

         // Update all tabs to make sure their names are correct.
         int count = mGraphTabs->count();
         for (int index = 0; index < count; index++)
         {
            EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
            if (view && view->GetScene()->GetGraph() == director->GetGraphData())
            {
               mGraphTabs->setTabText(index, director->GetGraphData()->GetName().c_str());
            }
         }

         return;
      }
   }


} // namespace dtDirector

