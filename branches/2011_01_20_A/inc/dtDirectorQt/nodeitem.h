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

#ifndef DIRECTORQT_NODE_ITEM
#define DIRECTORQT_NODE_ITEM

#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QWidget>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <dtDirector/node.h>


#define LINK_SIZE    12
#define LINK_LENGTH  15
#define LINK_PADDING 5

#define MAX_VALUE_NAME_SIZE 80

#define MIN_NODE_WIDTH  60
#define MIN_NODE_HEIGHT 50

#define MAX_NODE_WIDTH  500


namespace dtDirector
{
   class EditorScene;
   class InputLinkItem;
   class OutputLinkItem;
   class ValueLinkItem;
   class ValueNodeLinkItem;

   /**
    * Inherited Graphics Text Item that ignores context menu events.
    */
   class GraphicsTextItem: public QGraphicsTextItem
   {
   public:
      GraphicsTextItem(QGraphicsItem* parent, QGraphicsScene* scene)
         : QGraphicsTextItem(parent, scene) {}
   protected:
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
      {
         event->ignore();
      }
   };

   struct InputData
   {
      InputData();
      ~InputData();

      void Remove();

      GraphicsTextItem*    linkName;
      InputLinkItem*       linkGraphic;
      InputLink*           link;
   };

   struct OutputData
   {
      OutputData();
      ~OutputData();
      void ResizeLinks(int count, EditorScene* scene);

      void Remove(EditorScene* scene);

      GraphicsTextItem*    linkName;
      OutputLinkItem*      linkGraphic;
      std::vector<QGraphicsPathItem*> linkConnectors;
      OutputLink*          link;
   };

   struct ValueData
   {
      ValueData();
      ~ValueData();
      void ResizeLinks(int count, EditorScene* scene);

      void Remove(EditorScene* scene);

      GraphicsTextItem*    linkName;
      ValueLinkItem*       linkGraphic;
      std::vector<QGraphicsPathItem*> linkConnectors;
      ValueLink*           link;
   };

   /**
    * Draws a node in the graphics view.
    */
   class NodeItem
      : public QWidget
      , public QGraphicsPolygonItem
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
      NodeItem(Node* node, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Destructor.
       */
      virtual ~NodeItem();

      /**
       * Draws the node.
       */
      virtual void Draw();

      /**
       * Draws the polygon's top geometry.
       *
       * @note  This should always be the first Draw method
       *        called after the width and height have been
       *        determined.
       */
      void DrawPolygonTop();

      /**
       * Draws the polygon's right side geometry as a rounded
       * bullet.
       *
       * @note  This should always be the second draw method
       *        called and will replace the DrawPolygonRightFlat
       *        method.
       */
      void DrawPolygonRightRound();

      /**
       * Draws the polygon's right side geometry as a flat surface.
       *
       * @note  This should always be the second draw method
       *        called and will replace the DrawPolygonRightRound
       *        method.
       */
      void DrawPolygonRightFlat();

      /**
       * Draws the polygon's bottom geometry as a rounded bullet.
       *
       * @note  This should always be the third draw method
       *        called and will replace the DrawPolygonBottomFlat
       *        method.
       */
      void DrawPolygonBottomRound();

      /**
       * Draws the polygon's bottom geometry as a flat surface.
       *
       * @note  This should always be the third draw method
       *        called and will replace the DrawPolygonBottomRound
       *        method.
       */
      void DrawPolygonBottomFlat();

      /**
       * Draws the polygon's left side geometry as a round bullet.
       *
       * @note  This should always be the last draw method
       *        called and will replace the DrawPolygonLeftFlat
       *        method.
       */
      void DrawPolygonLeftRound();

      /**
       * Draws the polygon's left side geometry as a flat surface.
       *
       * @note  This should always be the last draw method
       *        called and will replace the DrawPolygonLeftRound
       *        method.
       */
      void DrawPolygonLeftFlat();

      /**
       * Converts a degree angle to a radian.
       */
      float DegToRad(float deg)
      {
         return deg / 180.0f * 3.14159265f;
      }

      /**
       * Rotates a point around the origin.
       *
       * @param[in]  point  The point to rotate.
       * @param[in]  angle  The angle in radians.
       *
       * @return     A new rotated point.
       */
      QPointF RotatePoint(const QPointF& point, float angle)
      {
         return QPointF(cos(angle) * point.x() - sin(angle) * point.y(),
                        sin(angle) * point.x() + cos(angle) * point.y());
      }

      /**
       * Create Title
       *
       * @param[in]  text  The new title name.
       */
      virtual void SetTitle(std::string text);

      /**
       * Create Title
       *
       * @param[in]  node  The node to get title from.
       */
      virtual std::string GetNodeTitle();

      /**
       * Creates the comment.
       *
       * @param[in]  text  The new comment text.
       */
      virtual void SetComment(const std::string& text);

      /**
       * Finds all of the links.
       */
      virtual void FindLinks();

      /**
       * Draws the title of the node.
       */
      void DrawTitle();

      /**
       * Draws the inputs of the node.
       */
      void DrawInputs();

      /**
       * Draw the outputs of the node.
       */
      void DrawOutputs();

      /**
       * Sets up the value link data.
       */
      void SetupValues();

      /**
       * Draw the value links of the node.
       */
      void DrawValues();

      /**
       * Draws the dividers.
       */
      void DrawDividers();

      /**
       * Retrieves the color for a given property type.
       *
       * @param[in]  type  The type.
       *
       * @return     A color.
       */
      //QColor GetColorForType(unsigned char type);
      //QColor GetDarkColorForType(unsigned char type);

      /**
       * Retrieves the node.
       *
       * @return  The node.
       */
      Node* GetNode() {return mNode.get();}

      /**
       * Retrieves the position of the node.
       */
      virtual osg::Vec2 GetPosition();

      /**
       * Retrieves the width of the node.
       */
      float GetNodeWidth() {return mNodeWidth;}

      /**
       * Retrieves the height of the node.
       */
      float GetNodeHeight() {return mNodeHeight;}

      /**
       * Retrieves the ID of the node item contained.
       *
       * @return  The ID.
       */
      virtual dtCore::UniqueId GetID();

      /**
       * Checks if this item contains a node of the given ID.
       *
       * @param[in]  id  The ID to check.
       */
      virtual bool HasID(const dtCore::UniqueId& id);

      /**
       * Retrieves the links.
       */
      std::vector<InputData>&  GetInputs() {return mInputs;}
      std::vector<OutputData>& GetOutputs() {return mOutputs;}
      std::vector<ValueData>&  GetValues() {return mValues;}

      /**
       * Connects all links on this node.
       *
       * @param[in]  fullConnect  True to perform a full
       *                          connection including input
       *                          links.
       */
      virtual void ConnectLinks(bool fullConnect = false);

      /**
       * Creates a path item for a connection link.
       *
       * @param[in]  start        The starting position.
       * @param[in]  end          The ending position.
       * @param[in]  drawReverse  Draws the link in reverse.
       *
       * @return     The resulting painter path.
       */
      QPainterPath CreateConnectionH(QPointF start, QPointF end, bool drawReverse = false);
      QPainterPath CreateConnectionV(QPointF start, QPointF end, bool drawReverse = false);

      /**
       * Event handlers when the user is moving this node.
       */
      virtual void BeginMoveEvent();
      virtual void EndMoveEvent();

      /**
       * Event handler when a child of this item changes.
       */
      virtual void childItemChange(QGraphicsItem* changedItem, GraphicsItemChange change, const QVariant &value) {}

      /**
       *	get qt-color value from node color-property
       */
      virtual QColor GetNodeColor() const;


      static const QColor LINE_COLOR;

   public slots:

      /**
       * Event handler when a value link is being exposed.
       */
      void ExposeValue(QAction* action);

   protected:

      /**
       * Event handler when the context menu event has triggered.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      /**
       * Event handler when this item changes.
       */
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);

      /**
       * Re-implementations of functions that are ambiguous to both
       * inherited classes.
       */
      void setToolTip(const QString &tip) {QGraphicsPolygonItem::setToolTip(tip);}
      void mousePressEvent(QGraphicsSceneMouseEvent *event) {QGraphicsPolygonItem::mousePressEvent(event);}
      void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {QGraphicsPolygonItem::mouseReleaseEvent(event);}
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {QGraphicsPolygonItem::mouseDoubleClickEvent(event);}

      /**
       * Connects an output to an input.
       *
       * @param[in]  output  The output.
       * @param[in]  input   The input.
       * @param[in]  index   The output link index.
       */
      void ConnectLinks(OutputData& output, InputData& input, int index, bool isOutput);

      /**
       * Connects a value link to a value node.
       *
       * @param[in]  output  The value link data.
       * @param[in]  input   The node item.
       * @param[in]  index   The value link index.
       */
      void ConnectLinks(ValueData& output, NodeItem* input, int index);

      /**
       *	Set a gradient brush.
       */
      void SetBackgroundGradient(unsigned int gradientHeight);

      /**
       *	Set a dark foreground pen.
       */
      void SetDefaultPen();

   protected:

      EditorScene* mScene;
      bool         mLoading;

      std::string  mOldPosition;

      QPolygonF   mPolygon;

      GraphicsTextItem* mTitle;
      QGraphicsRectItem* mTitleBG;

      GraphicsTextItem* mComment;

      dtCore::RefPtr<Node> mNode;

      std::vector<InputData>  mInputs;
      std::vector<OutputData> mOutputs;
      std::vector<ValueData>  mValues;

      QGraphicsRectItem* mLinkDivider;
      QGraphicsRectItem* mValueDivider;
      QGraphicsRectItem* mTitleDivider;

      int         mNodeWidth;
      int         mNodeHeight;

      float       mTitleHeight;

      float       mLinkWidth;
      float       mLinkHeight;

      float       mValueHeight;

      bool        mHasHiddenLinks;

   };
}

#endif
