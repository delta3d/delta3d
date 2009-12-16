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

#ifndef DIRECTORQT_VALUE_ITEM
#define DIRECTORQT_VALUE_ITEM

#include <dtDirectorQt/nodeitem.h>

namespace dtDirector
{
   class ValueNodeLinkItem;

   /**
    * Draws a node in the graphics view.
    */
   class ValueItem : public NodeItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  node    The Director Node.
       * @param[in]  parent  The parent item.
       * @param[in]  scene   The scene.
       */
      ValueItem(Node* node, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Draws the node.
       */
      virtual void Draw();

      /**
       * Create Title
       *
       * @param[in]  text  The new title name.
       */
      virtual void SetTitle(std::string text);

      /**
       * Creates the value text.
       *
       * @param[in]  text  The value text.
       */
      virtual void SetValueText(const std::string& text);

      /**
       * Creates the comment.
       *
       * @param[in]  text  The new comment text.
       */
      virtual void SetComment(const std::string& text);

      /**
       * Connects all links on this node.
       *
       * @param[in]  fullConnect  True to perform a full
       *                          connection including input
       *                          links.
       */
      virtual void ConnectLinks(bool fullConnect = false);

      /**
       * Retrieves the value link item.
       */
      ValueNodeLinkItem* GetValueLink() {return mValueLink;}


      friend class ValueNodeLinkItem;

   protected:

      /**
       * Event handler when the context menu event has triggered.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      GraphicsTextItem*  mValueText;

      ValueNodeLinkItem* mValueLink;
   };
}

#endif
