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

#ifndef DIRECTORQT_GROUP_ITEM
#define DIRECTORQT_GROUP_ITEM

#include <dtDirectorQt/nodeitem.h>

#define AUTO_BORDER_SIZE 20

namespace dtDirector
{
   class LockItem;
   class ResizeItem;

   /**
    * Draws a node in the graphics view.
    */
   class GroupItem
      : public NodeItem
   {
      Q_OBJECT
   public:

      /**
       * Constructor.
       *
       * @param[in]  node    The Director Node.
       * @param[in]  parent  The parent item.
       * @param[in]  scene   The scene.
       */
      GroupItem(Node* node, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Draws the node.
       */
      virtual void Draw();

      /**
       * Event handlers when the user is moving this node.
       */
      virtual void BeginMoveEvent();
      virtual void EndMoveEvent();

      /**
       * Event handler when a child of this item changes.
       */
      virtual void childItemChange(QGraphicsItem* changedItem, GraphicsItemChange change, const QVariant &value);

      /**
       * Sizes the group to fit a given list of node items.
       *
       * @param[in]  nodeItems  The node items to size to.
       */
      void SizeToFit(const QList<NodeItem*>& nodeItems);

   public slots:

      /**
       * Event handler that will resize the group to fit items currently contained.
       */
      void SizeToFit();
      
   protected:

      /**
       * Event handler when the context menu event has triggered.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      /**
       * Event handler when this item changes.
       */
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);

      LockItem*   mLocker;
      ResizeItem* mResizer;

      QList<NodeItem*> mMovingNodes;
   };
}

#endif
