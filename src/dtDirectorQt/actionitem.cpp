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

#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirector/director.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ActionItem::ActionItem(Node* node, QGraphicsItem* parent, EditorScene* scene)
       : NodeItem(node, parent, scene)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ActionItem::Draw()
   {
      NodeItem::Draw();

      mLoading = true;

      if (mNode.valid())
      {
         SetTitle(mNode->GetName());
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

         QRadialGradient radialGradient(mNodeWidth/2, mNodeHeight/2, size, mNodeWidth/2, mNodeHeight/2);

         if (mNode->GetEnabled())
         {
            if (mNode->GetNodeLogging())
            {
               setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            }
            else
            {
               setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            }

            QColor color = Qt::darkCyan;
            color.setAlphaF(0.80f);
            radialGradient.setColorAt(0.0, color);

            color = Qt::cyan;
            color.setAlphaF(0.80f);
            radialGradient.setColorAt(1.0, color);
         }
         else
         {
            setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            QColor color = Qt::darkCyan;
            color.setAlphaF(0.25f);
            radialGradient.setColorAt(0.0, color);

            color = Qt::cyan;
            color.setAlphaF(0.25f);
            radialGradient.setColorAt(1.0, color);
         }

         // If we are in replay mode and if this node is the current node,
         // make sure we highlight it with a special color.
         if (mScene->GetEditor()->GetReplayMode() &&
            mScene->GetEditor()->GetReplayNode().nodeID == mNode->GetID())
         {
            setPen(QPen(Qt::yellow, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         }

         setBrush(radialGradient);
         setPolygon(mPolygon);

         SetComment(mNode->GetComment());
      }

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActionItem::OnGotoEvent()
   {
      dtDAL::ActorProperty* prop = mNode->GetProperty("EventName");
      if (prop)
      {
         // Find the referenced value node.
         std::string name = prop->ToString();

         std::vector<Node*> nodes;
         mScene->GetEditor()->GetDirector()->GetNodes("Remote Event", "Core", "EventName", name, nodes);

         bool foundCurrent = false;
         if (mNode->GetType().GetFullName() == "Core.Call Remote Event")
         {
            foundCurrent = true;
         }

         Node* jumpNode = NULL;
         int count = (int)nodes.size();
         for (int index = 0; index < count; index++)
         {
            Node* node = nodes[index];
            if (!node) continue;

            // By default, it will jump to the first node.
            if (!jumpNode) jumpNode = node;

            // If we have already found the current event,
            // then this even is the one we want to jump to.
            if (foundCurrent)
            {
               jumpNode = node;
               break;
            }

            if (node == mNode.get()) foundCurrent = true;
         }

         // Now jump the view to the found event node.
         if (jumpNode)
         {
            // Center the view on the referenced node.
            EditorScene* scene = mScene;
            scene->SetGraph(jumpNode->GetGraph());
            NodeItem* item = scene->GetNodeItem(jumpNode->GetID(), true);
            if (item)
            {
               scene->clearSelection();
               item->setSelected(true);
               scene->CenterSelection();
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActionItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);
      QMenu menu;
      QAction* jumpAction = NULL;
      if (mNode->GetType().GetFullName() == "Core.Remote Event")
      {
         jumpAction = menu.addAction("Go to next Remote Event");
      }
      else if (mNode->GetType().GetFullName() == "Core.Call Remote Event")
      {
         jumpAction = menu.addAction("Go to Remote Event");
      }

      if (jumpAction)
      {
         menu.setDefaultAction(jumpAction);
         connect(jumpAction, SIGNAL(triggered()), this, SLOT(OnGotoEvent()));
         menu.addSeparator();
      }

      if (mNode->GetType().GetFullName() == "Core.Input Link" ||
         mNode->GetType().GetFullName() == "Core.Output Link")
      {
         menu.addAction(mScene->GetEditor()->GetParentAction());
         menu.setDefaultAction(mScene->GetEditor()->GetParentAction());
         menu.addSeparator();
      }

      menu.addAction(mScene->GetMacroSelectionAction());
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
   void ActionItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
   {
      NodeItem::mouseDoubleClickEvent(event);

      // Double clicking a remote event or call remote event node will
      // jump the view to the next remote event found.
      if (mNode->GetType().GetFullName() == "Core.Remote Event" ||
         mNode->GetType().GetFullName() == "Core.Call Remote Event")
      {
         OnGotoEvent();
      }
      else if (mNode->GetType().GetFullName() == "Core.Input Link" ||
         mNode->GetType().GetFullName() == "Core.Output Link")
      {
         mScene->GetEditor()->OnParentButton();
      }
   }
}

//////////////////////////////////////////////////////////////////////////
