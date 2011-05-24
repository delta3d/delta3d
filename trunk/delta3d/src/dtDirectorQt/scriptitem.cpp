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
   ScriptItem::ScriptItem(Node* node, QGraphicsItem* parent, EditorScene* scene)
      : ActionItem(node, parent, scene)
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
         SetBackgroundGradient(mNodeHeight);

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
      dtDirector::EditorNotifier* notifier = 
         dynamic_cast<dtDirector::EditorNotifier*>(
         mScene->GetEditor()->GetDirector()->GetNotifier());

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

         if (!mGlowEffect && maxGlow > 0.0f)
         {
            mGlowEffect = new QGraphicsColorizeEffect();
            mGlowEffect->setColor(Qt::white);
            mGlowEffect->setStrength(0.0f);
            QGraphicsPolygonItem::setGraphicsEffect(mGlowEffect);
         }

         if (mGlowEffect)
         {
            mGlowEffect->setStrength(maxGlow);
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
      if (!mScene->GetSelection().empty())
      {
         QAction* createGroupAction = menu.addAction("Create Group Around Selection");
         connect(createGroupAction, SIGNAL(triggered()), mScene, SLOT(OnCreateGroupForSelection()));
      }
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();

      QMenu* exposeMenu = NULL;
      std::vector<ValueLink> &values = mNode->GetValueLinks();
      int count = (int)values.size();
      for (int index = 0; index < count; index++)
      {
         ValueLink& link = values[index];
         if (!link.GetExposed())
         {
            if (!exposeMenu)
            {
               exposeMenu = menu.addMenu("Expose Values");
               connect(exposeMenu, SIGNAL(triggered(QAction*)), this, SLOT(ExposeValue(QAction*)));
            }

            exposeMenu->addAction(link.GetName().c_str());
         }
      }

      menu.addAction(mScene->GetEditor()->GetShowLinkAction());
      menu.addAction(mScene->GetEditor()->GetHideLinkAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetDeleteAction());
      menu.exec(event->screenPos());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ScriptItem::HasNode(Node* node)
   {
      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script && script->GetGraphRoot())
         {
            std::vector<Node*> nodes;
            script->GetGraphRoot()->GetAllNodes(nodes, false);

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

      EditScript();
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
               dtDirector::EditorNotifier* notifier = 
                  dynamic_cast<dtDirector::EditorNotifier*>(script->GetNotifier());
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
