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

#ifndef DIRECTORQT_RESIZE_ITEM
#define DIRECTORQT_RESIZE_ITEM

#include <dtDirector/node.h>
#include <dtDirectorQt/nodeitem.h>

#include <QtGui/QGraphicsPolygonItem>


namespace dtDirector
{
   class EditorScene;
   class GroupItem;

   /**
    * Handles an input link in the UI.
    */
   class ResizeItem
      : public QGraphicsPolygonItem
   {
   public:

      enum ResizeType
      {
         RESIZE_TOP_LEFT,
         RESIZE_TOP_RIGHT,
         RESIZE_BOT_LEFT,
         RESIZE_BOT_RIGHT,
         RESIZE_TOP,
         RESIZE_BOT,
         RESIZE_LEFT,
         RESIZE_RIGHT,
         RESIZE_COUNT,
      };

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       * @param[in]  type       The resizer type.
       */
      ResizeItem(GroupItem* nodeItem, QGraphicsItem* parent, EditorScene* scene, ResizeType type);

      /**
       * Initializes the item.
       */
      void Init();

      /**
       * Retrieves the desired position for this sizer.
       */
      QPointF GetFramePosition();

      /**
       * Highlights the link.
       *
       * @param[in]  enable  True to highlight.
       */
      void SetHighlight(bool enable);

      /**
       * Retrieves the resize items type.
       */
      ResizeType GetType() const {return mType;}

   protected:

      /**
       * Event handler when this item changes.
       */
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

      /**
       * Event handlers when the user presses a mouse button.
       *
       * @param[in]  mouseEvent  The mouse event.
       */
      void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

      /**
       * Event handler when the user releases the mouse.
       *
       * @param[in]  mouseEvent  The mouse event.
       */
      void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

      /**
       * Event handler when the user moves the mouse.
       *
       * @param[in]  mouseEvent  The mouse event.
       */
      void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);


      EditorScene*   mScene;
      GroupItem*     mNodeItem;

      ResizeType     mType;
   };
}

#endif
