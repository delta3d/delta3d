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
   PropertyEditor::PropertyEditor(QMainWindow* parent)
      : dtQt::BasePropertyEditor(parent)
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
         EventNode* eventNode = dynamic_cast<EventNode*>(selectedContainers[0]);
         if (eventNode)
         {
            return "Event Node '" + tr(eventNode->GetType().GetFullName().c_str()) + "' selected";
         }

         ActionNode* actionNode = dynamic_cast<ActionNode*>(selectedContainers[0]);
         if (actionNode)
         {
            return "Action Node '" + tr(actionNode->GetType().GetFullName().c_str()) + "' selected";
         }

         ValueNode* valueNode = dynamic_cast<ValueNode*>(selectedContainers[0]);
         if (valueNode)
         {
            return "Value Node '" + tr(valueNode->GetType().GetFullName().c_str()) + "' selected";
         }

         DirectorGraphData* graph = dynamic_cast<DirectorGraphData*>(selectedContainers[0]);
         if (graph)
         {
            return "Macro '" + tr(graph->GetName().c_str()) + "' selected";
         }

         Director* director = dynamic_cast<Director*>(selectedContainers[0]);
         if (director)
         {
            return "Director '" + tr(director->GetName().c_str()) + "' selected";
         }
      }

      return BaseClass::GetGroupBoxLabelText(baseGroupBoxName);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::buildDynamicControls(dtDAL::PropertyContainer& propCon, dtQt::DynamicGroupControl* parentControl)
   {
      dtQt::DynamicGroupControl* parent = GetRootControl();
      if (parentControl != NULL)
      {
         parent = parentControl;
      }

      std::vector<dtDAL::ActorProperty*> propList;
      propCon.GetPropertyList(propList);

      //ViewportManager::GetInstance().emitModifyPropList(propCon, propList);

      BaseClass::buildDynamicControls(propCon, propList, parentControl);
   }

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

         return;
      }

      // Check if the container is the director.
      Director* director = dynamic_cast<Director*>(&propCon);
      if (director)
      {
         return;
      }
   }


} // namespace dtDirector

