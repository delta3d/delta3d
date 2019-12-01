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
#include <QtGui/QPen>

namespace dtDirector
{
   class EditorScene;

   class LinkItem
      : public QWidget
      , public QGraphicsPolygonItem
   {
      Q_OBJECT

      public:

         /**
          * Retrieves the owner node item.
          */
         NodeItem* GetNodeItem() {return mNodeItem;}

         /**
          * Sets the pen color.
          */
         void SetPenColor(const QColor& color);

         /**
          * Highlights the link.
          *
          * @param[in]  enable  True to highlight.
          */
         virtual void SetHighlight(bool enable);

         /**
          * Sets the link to always highlight.
          *
          * @param[in]  enabled  True to always highlight.
          */
         void SetAlwaysHighlight(bool enabled);

         /**
          * Sets the highlight pen color alpha.
          */
         void SetHighlightAlpha(float alpha);

         /**
          * Retrieves whether this link should always highlight.
          */
         bool GetAlwaysHighlight() {return mAlwaysHighlight;}

         void SetHighlightConnector(bool enable, QGraphicsPathItem* connector, bool isReadOnly = false, bool isRemoved = false);

         static const unsigned int LINE_WIDTH;

      signals:
         void LinkConnected();

      protected:
         LinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment);

         /**
          * Event handlers when the user presses a mouse button.
          *
          * @param[in]  mouseEvent  The mouse event.
          */
         void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

         virtual LinkItem* GetLinkGraphic() = 0;

         QGraphicsPathItem* mDrawing;
         QGraphicsPathItem* mHighlight;

         QPen mLinkGraphicPen;
         QPen mHighlightPen;
         EditorScene*   mScene;

         int            mLinkIndex;

         bool           mAltModifier;
         bool           mAlwaysHighlight;

         NodeItem*      mNodeItem;
   };

   /**
    * Handles an input link in the UI.
    */
   class InputLinkItem : public LinkItem
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  linkIndex  The input link index.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       * @param[in]  comment    The comment.
       */
      InputLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment);

      /**
       * Draws the glow effect on this link.
       */
      void DrawGlow(float glow);

      /**
       * Sets the default highlight on the link.
       */
      //void InitHighlight();

      /**
       * Disconnects a link.
       *
       * @param[in]  output  The link to disconnected, use NULL to
       *                     disconnect all.
       */
      void Disconnect(OutputLink* output = NULL);

      friend class OutputLinkItem;

   public slots:

      /**
       * Event handler when a link is disconnected.
       *
       * @param[in]  action  The disconnect action.
       */
      void Disconnect(QAction* action);

      /**
       * Event handler when a link is hidden.
       */
      void HideLink();
      void ShowLink();

   protected:
      virtual LinkItem* GetLinkGraphic();

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);


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

      /**
       * Event handler for the context menu event.
       *
       * @param[in]  event  The event.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
   };


   /**
    * Handles an output link in the UI.
    */
   class OutputLinkItem : public LinkItem
   {
      Q_OBJECT
   public:

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  linkIndex  The output link index.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       * @param[in]  comment    The comment.
       */
      OutputLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment);

      /**
       * Draws the glow effect on this link.
       */
      void DrawGlow(float glow);

      /**
       * Highlights the link.
       *
       * @param[in]  enable     True to highlight.
       * @param[in]  inputLink  The input link that should highlight
       *                         with it, or NULL to highlight all.
       */
      void SetHighlight(bool enable, InputLink* inputLink = NULL);

      /**
       * Disconnects a link.
       *
       * @param[in]  input  The link to disconnected, use NULL to
       *                     disconnect all.
       */
      void Disconnect(InputLink* input = NULL);

      friend class InputLinkItem;

   public slots:

      /**
       * Event handler when a link is disconnected.
       *
       * @param[in]  action  The disconnect action.
       */
      void Disconnect(QAction* action);

      /**
       * Event handler when a link is hidden.
       */
      void HideLink();
      void ShowLink();

   protected:
      virtual LinkItem* GetLinkGraphic();

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);


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

      /**
       * Event handler for the context menu event.
       *
       * @param[in]  event  The event.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
   };

   /**
    * Handles a value link in the UI.
    */
   class ValueLinkItem : public LinkItem
   {
      Q_OBJECT
   public:

      /**
       * Constructor.
       *
       * @param[in]  nodeItem   The node item.
       * @param[in]  linkIndex  The value link index.
       * @param[in]  parent     The parent item.
       * @param[in]  scene      The scene.
       * @param[in]  comment    The comment.
       */
      ValueLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment);

      /**
       * Sets the type of the link property.
       *
       * @param[in]  type  The type.
       */
      void SetPropertyType(unsigned char type) {mType = type;}
      unsigned char GetPropertyType() {return mType;}

      /**
       * Highlights the link.
       *
       * @param[in]  enable     True to highlight.
       * @param[in]  valueNode  The value node that should highlight
       *                         with it, or NULL to highlight all.
       */
      void SetHighlight(bool enable, Node* valueNode = NULL);

      /**
       * Disconnects a link.
       *
       * @param[in]  input  The link to disconnected, use NULL to
       *                     disconnect all.
       */
      void Disconnect(ValueNode* output = NULL);

      friend class ValueNodeLinkItem;

   public slots:

      /**
       * Event handler when a link is disconnected.
       *
       * @param[in]  action  The disconnect action.
       */
      void Disconnect(QAction* action);

      /**
       * Event handler when a link is hidden.
       */
      void HideLink();
      void ShowLink();

      /**
       * Event handler when a link is removed.
       */
      void RemoveLink();

   protected:
      virtual LinkItem* GetLinkGraphic();

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

      /**
       *	Creates a new Value Node and connects it to this link.
       * This new node will automatically be linked to this link.
       *
       * @param[in]  newPos        The position to place the new node.
       * @param[in]  refreshViews  Whether we want to refresh the editor views.
       *
       * @return     A pointer to the newly created value node item.
       */
      ValueNode* CreateValueNode(QPointF newPos, bool refreshViews);

      /**
       * Event handler when the user releases the mouse.
       *
       * @param[in]  mouseEvent  The mouse event.
       */
      void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

      /**
       *	Event handler when the user double clicks the mouse.
       *
       * @param[in]  mouseEvent  The mouse event.
       */
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

      /**
       * Event handler when the user moves the mouse.
       *
       * @param[in]  mouseEvent  The mouse event.
       */
      void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);

      /**
       * Event handler for the context menu event.
       *
       * @param[in]  event  The event.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      unsigned char  mType;
   };

   /**
    * Handles a value node link in the UI.
    */
   class ValueNodeLinkItem: public LinkItem
   {
      Q_OBJECT
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
      void SetPropertyType(unsigned char type) {mType = type;}
      unsigned char GetPropertyType() {return mType;}

      /**
       * Disconnects a link.
       *
       * @param[in]  input  The link to disconnected, use NULL to
       *                     disconnect all.
       */
      void Disconnect(ValueLink* input = NULL);

      friend class ValueLinkItem;

   public slots:

      /**
       * Event handler when a link is disconnected.
       *
       * @param[in]  action  The disconnect action.
       */
      void Disconnect(QAction* action);

   protected:
      virtual LinkItem* GetLinkGraphic();

      /**
       * Event handler when the user hovers their mouse
       * over this item.
       */
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);


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

      /**
       * Event handler for the context menu event.
       *
       * @param[in]  event  The event.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      unsigned char mType;
   };
}

#endif
