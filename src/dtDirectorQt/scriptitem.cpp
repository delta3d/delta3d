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
#include <dtDirectorQt/scriptitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/editornotifier.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <dtDirectorNodes/referencescriptaction.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsColorizeEffect>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ScriptItem::ScriptItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent, EditorScene* scene)
      : ActionItem(node, readOnly, imported, parent, scene)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::Draw()
   {
      // Refresh the links just in case.
      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());
      if (refScriptNode)
      {
         refScriptNode->RefreshLinks();
      }

      NodeItem::Draw();

      mLoading = true;

      if (mNode.valid())
      {
         SetTitle(GetNodeTitle());
         if (mNode->InputsExposed())  DrawInputs();
         if (mNode->ValuesExposed())  SetupValues();
         if (mNode->OutputsExposed()) DrawOutputs();

         // Now draw the node.
         DrawTitle();
         DrawValues();

         DrawPolygonTop();
         if (mNode->OutputsExposed()) DrawPolygonRightFlat();
         else                         DrawPolygonRightRound();
         DrawPolygonBottomFlat();
         if (mNode->InputsExposed()) DrawPolygonLeftFlat();
         else                        DrawPolygonLeftRound();

         DrawDividers();

         int size = mNodeWidth;
         if (size < mNodeHeight) size = mNodeHeight;

         setPolygon(mPolygon);

         SetComment(mNode->GetComment());

         SetDefaultPen();
         SetBackgroundGradient();

         DrawGlow();
      }

      mLoading = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ScriptItem::DrawGlow()
   {
      if (!mScene)
      {
         return;
      }

      float maxGlow = 0.0f;
      EditorNotifier* notifier = mScene->GetEditor()->GetNotifier();

      // Update the glow of this item only if a node inside it is glowing.
      DirectorGraph* graph = NULL;
      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script && script->GetGraphRoot())
         {
            graph = script->GetGraphRoot();
         }
      }

      if (notifier && graph)
      {
         std::vector<Node*> nodes;
         graph->GetAllNodes(nodes);

         int count = (int)nodes.size();
         for (int index = 0; index < count; ++index)
         {
            Node* node = nodes[index];
            if (node)
            {
               dtDirector::EditorNotifier::GlowData* glowData =
                  notifier->GetGlowData(node);

               if (glowData && glowData->glow > maxGlow)
               {
                  maxGlow = glowData->glow;
               }
            }
         }

         if (maxGlow > 0.5f)
         {
            SetHighlight(1.0f);
         }
         else
         {
            SetHighlight(maxGlow * 2.0f);
         }

         std::vector<dtCore::RefPtr<EventNode> > inputs = graph->GetInputNodes();
         count = (int)inputs.size();
         int inputIndex = 0;
         for (int index = 0; index < count; index++)
         {
            if (inputs[index]->IsEnabled())
            {
               dtDirector::EditorNotifier::GlowData* glowData =
                  notifier->GetGlowData(inputs[index]);
               if (glowData && glowData->input > -1)
               {
                  InputData& data = mInputs[inputIndex];
                  data.DrawGlow(glowData->inputGlow);
               }

               inputIndex++;
            }
         }

         std::vector<dtCore::RefPtr<ActionNode> > outputs = graph->GetOutputNodes();
         count = (int)outputs.size();
         int outputIndex = 0;
         for (int index = 0; index < count; index++)
         {
            if (outputs[index]->IsEnabled())
            {
               dtDirector::EditorNotifier::GlowData* glowData =
                  notifier->GetGlowData(outputs[index]);
               if (glowData && !glowData->outputGlows.empty())
               {
                  OutputData& data = mOutputs[outputIndex];
                  data.DrawGlow(glowData->outputGlows[0]);
               }

               outputIndex++;
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);

      QMenu menu;
      QAction* editScriptAction = NULL;

      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script && script->GetGraphRoot())
         {
            editScriptAction = menu.addAction("Edit Script");
            connect(editScriptAction, SIGNAL(triggered()), this, SLOT(EditScript()));
            menu.setDefaultAction(editScriptAction);
         }
      }

      menu.addAction(mScene->GetMacroSelectionAction());
      menu.addAction(mScene->GetGroupSelectionAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();
      menu.addMenu(GetLinkVisibilityMenu());
      menu.addSeparator();
      menu.addMenu(GetExposeLinkMenu());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetDeleteAction());
      menu.exec(event->screenPos());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ScriptItem::HasNode(Node* node)
   {
      if (node == GetNode())
      {
         return true;
      }

      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script && script->GetGraphRoot())
         {
            std::vector<Node*> nodes;
            script->GetGraphRoot()->GetAllNodes(nodes);

            int count = (int)nodes.size();
            for (int index = 0; index < count; index++)
            {
               Node* testNode = nodes[index];

               if (testNode == node) return true;
            }
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
   {
      ActionItem::mouseDoubleClickEvent(event);

      if (event->button() == Qt::LeftButton)
      {
         EditScript();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::EditScript()
   {
      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script)
         {
            dtDirector::DirectorEditor* editor = new dtDirector::DirectorEditor();
            if (editor)
            {
               EditorNotifier* notifier = mScene->GetEditor()->GetNotifier();
               if (notifier)
               {
                  notifier->AddEditor(editor);
               }

               editor->SetDirector(script);
               editor->show();
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
