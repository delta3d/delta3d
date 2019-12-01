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

#ifndef DIRECTORQT_ACTION_ITEM
#define DIRECTORQT_ACTION_ITEM

#include <dtDirectorQt/nodeitem.h>

namespace dtDirector
{
   /**
    * Draws a node in the graphics view.
    */
   class ActionItem : public NodeItem
   {
      Q_OBJECT
   public:

      /**
       * Constructor.
       *
       * @param[in]  node      The Director Node.
       * @param[in]  readOnly  True if this node is read only.
       * @param[in]  imported  True if this node is an imported node.
       * @param[in]  parent    The parent item.
       * @param[in]  scene     The scene.
       */
      ActionItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Draws the node.
       */
      virtual void Draw();

   public slots:

      /**
       * Event handler when you want to jump to an event.
       */
      void OnGotoEvent();

   protected:

      /**
       * Event handler when the context menu event has triggered.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      /**
       * Event handler when the user double clicks this node.
       *
       * @param[in]  event  The mouse event.
       */
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

   };
}

#endif
