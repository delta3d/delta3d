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

#ifndef DIRECTORQT_LINK_ITEM
#define DIRECTORQT_LINK_ITEM

#include <dtDirector/node.h>
#include <dtDirectorQt/nodeitem.h>
#include <dtDirectorQt/valueitem.h>

#include <QtGui/QGraphicsPolygonItem>


namespace dtDirector
{
   class EditorScene;

   /**
    * Handles an input link in the UI.
    */
   class InputLinkItem: public QGraphicsPolygonItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  linkIndex  The input link index.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       */
      InputLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene);

      /**
       * Highlights the link.
       *
       * @param[in]  enable  True to highlight.
       */
      void SetHighlight(bool enable);

   protected:

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

      EditorScene*   mScene;
      NodeItem*      mNodeItem;
      int            mLinkIndex;
      int            mLineWidth;
   };


   /**
    * Handles an output link in the UI.
    */
   class OutputLinkItem: public QGraphicsPolygonItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  linkIndex  The output link index.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       */
      OutputLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene);

      /**
       * Highlights the link.
       *
       * @param[in]  enable     True to highlight.
       * @param[in]  inputLink  The input link that should highlight
       *                         with it, or NULL to highlight all.
       */
      void SetHighlight(bool enable, InputLink* inputLink = NULL);

   protected:

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

      EditorScene*   mScene;
      NodeItem*      mNodeItem;
      int            mLinkIndex;
      int            mLineWidth;
   };

   /**
    * Handles a value link in the UI.
    */
   class ValueLinkItem: public QGraphicsPolygonItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  linkIndex  The value link index.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       */
      ValueLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene);

      /**
       * Sets the type of the link property.
       *
       * @param[in]  type  The type.
       */
      void SetType(unsigned char type) {mType = type;}
      unsigned char GetType() {return mType;}

      /**
       * Highlights the link.
       *
       * @param[in]  enable     True to highlight.
       * @param[in]  valueNode  The value node that should highlight
       *                         with it, or NULL to highlight all.
       */
      void SetHighlight(bool enable, Node* valueNode = NULL);

   protected:

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

      EditorScene*   mScene;
      NodeItem*      mNodeItem;
      int            mLinkIndex;
      int            mLineWidth;

      unsigned char  mType;
   };

   /**
    * Handles a value node link in the UI.
    */
   class ValueNodeLinkItem: public QGraphicsPolygonItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  valueItem  The value node item.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       */
      ValueNodeLinkItem(ValueItem* valueItem, QGraphicsItem* parent, EditorScene* scene);

      /**
       * Sets the type of the link property.
       *
       * @param[in]  type  The type.
       */
      void SetType(unsigned char type) {mType = type;}
      unsigned char GetType() {return mType;}

      /**
       * Highlights the link.
       *
       * @param[in]  enable  True to highlight.
       */
      void SetHighlight(bool enable);

   protected:

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

      EditorScene*   mScene;
      ValueItem*     mValueItem;
      int            mLineWidth;

      unsigned char mType;
   };
}

#endif