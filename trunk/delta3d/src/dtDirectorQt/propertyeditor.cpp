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
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/nodeitem.h>
#include <dtDirectorQt/scriptitem.h>
#include <dtDirectorQt/macroitem.h>
#include <dtDirectorQt/valueitem.h>

#include <dtDirector/node.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////
   PropertyEditor::PropertyEditor(QMainWindow* parent)
      : dtQt::BasePropertyEditor(parent)
      , mScene(NULL)
      , mGraphTabs(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditor::~PropertyEditor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::SetDirectorEditor(DirectorEditor* editor)
   {
      mDirectorEditor = editor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditor::GetGroupBoxLabelText(const QString& baseGroupBoxName)
   {
      std::vector<dtCore::PropertyContainer*> selectedContainers;
      GetSelectedPropertyContainers(selectedContainers);

      if (selectedContainers.size() == 1)
      {
         return QString(GetContainerGroupName(selectedContainers[0]).c_str()) + " selected";
      }

      return BaseClass::GetGroupBoxLabelText("Multiple nodes selected");
   }

   //////////////////////////////////////////////////////////////////////////
   std::string PropertyEditor::GetContainerGroupName(dtCore::PropertyContainer* propertyContainer)
   {
      Node* node = dynamic_cast<Node*>(propertyContainer);
      if (node)
      {
         EventNode* eventNode = node->AsEventNode();
         if (eventNode)
         {
            return std::string("Event Node '") + eventNode->GetType().GetFullName().c_str() + "'";
         }

         ActionNode* actionNode = node->AsActionNode();
         if (actionNode)
         {
            return std::string("Action Node '") + actionNode->GetType().GetFullName().c_str() + "'";
         }

         ValueNode* valueNode = node->AsValueNode();
         if (valueNode)
         {
            return std::string("Value Node '") + valueNode->GetType().GetFullName().c_str() + "'";
         }
      }
      else
      {
         DirectorGraph* graph = dynamic_cast<DirectorGraph*>(propertyContainer);
         if (graph)
         {
            return std::string("Macro '") + graph->GetName().c_str() + "'";
         }
         else
         {
            Director* director = dynamic_cast<Director*>(propertyContainer);
            if (director)
            {
               return std::string("Director '") + director->GetName().c_str() + "'";
            }
         }
      }

      return "Unknown Container";
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::closeEvent(QCloseEvent* e)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::OnContainersSelected(const std::vector<dtCore::RefPtr<dtCore::PropertyContainer> >& selection)
   {
      bool editable = true;

      int count = (int)selection.size();
      for (int index = 0; index < count; ++index)
      {
         dtCore::PropertyContainer* propertyContainer = selection[index];
         if (!propertyContainer)
         {
            continue;
         }

         Node* node = dynamic_cast<Node*>(propertyContainer);
         if (node)
         {
            if (node->IsImported() || node->IsReadOnly())
            {
               editable = false;
               break;
            }
         }
         else
         {
            DirectorGraph* graph = dynamic_cast<DirectorGraph*>(propertyContainer);
            if (graph)
            {
               if (graph->IsImported() || graph->IsReadOnly())
               {
                  editable = false;
                  break;
               }
            }
            else
            {
               Director* director = dynamic_cast<Director*>(propertyContainer);
               if (director && director->IsImported())
               {
                  editable = false;
                  break;
               }
            }
         }
      }

      SetReadOnly(!editable);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyAboutToChangeFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop,
            const std::string& oldValue, const std::string& newValue)
   {
      mOldValue = oldValue;
      mNewValue = newValue;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyChangedFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop)
   {
      // Check if the container is a node.
      Node* node = dynamic_cast<Node*>(&propCon);
      if (node)
      {
         NodeItem* item = mScene->GetNodeItem(node->GetID());

         // If the item is a value node or a reference script, we have to refresh the entire scene
         if (dynamic_cast<ValueItem*>(item) || dynamic_cast<ScriptItem*>(item))
         {
            mScene->Refresh();
         }
         else
         {
            // Re-Draw the node.
            if (item)
            {
               item->Draw();
               item->ConnectLinks(true);
            }
         }
      }
      else
      {
         // Check if the container is a graph.
         DirectorGraph* graph = dynamic_cast<DirectorGraph*>(&propCon);
         if (graph)
         {
            MacroItem* item = mScene->GetGraphItem(graph->GetID());

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

            mDirectorEditor->RefreshGraphBrowser();
         }
         else
         {
            // Check if the container is the director.
            Director* director = dynamic_cast<Director*>(&propCon);
            if (director)
            {
               UpdateTitle();

               // Update all tabs to make sure their names are correct.
               int count = mGraphTabs->count();
               for (int index = 0; index < count; index++)
               {
                  EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
                  if (view && view->GetScene()->GetGraph() == director->GetGraphRoot())
                  {
                     mGraphTabs->setTabText(index, director->GetGraphRoot()->GetName().c_str());
                  }
               }
            }
         }
      }

      if (mOldValue != mNewValue)
      {
         dtDirector::ID id;
         std::string undoDescription = "Property modification for ";
         // Check if the container is for a graph.
         DirectorGraph* graph = dynamic_cast<DirectorGraph*>(&propCon);
         if (graph)
         {
            if (graph->GetEditor().empty())
            {
               undoDescription += "Macro Node \'" + graph->GetName() + "\'.";
            }
            else
            {
               undoDescription += "\'" + graph->GetEditor() + "\' Macro Node \'" + graph->GetName() + "\'.";
            }
            id = graph->GetID();
         }
         else
         {
            // Check if the container is for a node.
            Node* node = dynamic_cast<Node*>(&propCon);
            if (node)
            {
               undoDescription += "Node \'" + node->GetTypeName() + "\'.";
               id = node->GetID();
            }
            // If it is not a graph or a node, then it is the director.
            else
            {
               undoDescription += "Script.";
            }
         }

         dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mDirectorEditor, id, prop.GetName(), mOldValue, mNewValue);
         event->SetDescription(undoDescription);
         mDirectorEditor->GetUndoManager()->AddEvent(event.get());
      }
   }


} // namespace dtDirector

