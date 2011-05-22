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
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <dtDirectorNodes/referencescriptaction.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>

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
      }

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);

      QMenu menu;
      QAction* stepInAction = NULL;

      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script && script->GetGraphRoot())
         {
            stepInAction = menu.addAction("Step Inside Macro");
            connect(stepInAction, SIGNAL(triggered()), this, SLOT(OpenMacro()));
            menu.setDefaultAction(stepInAction);
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

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
   {
      ActionItem::mouseDoubleClickEvent(event);

      ReferenceScriptAction* refScriptNode =
         dynamic_cast<ReferenceScriptAction*>(GetNode());

      if (refScriptNode)
      {
         dtDirector::Director* script = refScriptNode->GetDirectorScript();

         if (script && script->GetGraphRoot())
         {
            if (!mScene) return;

            // Open the subgraph.
            mScene->GetEditor()->OpenGraph(script->GetGraphRoot());
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
