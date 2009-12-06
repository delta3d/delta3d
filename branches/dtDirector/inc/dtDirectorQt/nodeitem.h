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

#include <dtDirector/node.h>


#define LINK_SIZE    10
#define LINK_LENGTH  15
#define LINK_SPACING 0

#define MAX_VALUE_NAME_SIZE 80

#define MIN_NODE_WIDTH  50
#define MIN_NODE_HEIGHT 50

#define MAX_NODE_WIDTH  500


namespace dtDirector
{
   class EditorScene;
   class InputLinkItem;
   class OutputLinkItem;
   class ValueLinkItem;
   class ValueNodeLinkItem;

   struct InputData
   {
      InputData();
      ~InputData();

      void Remove();

      QGraphicsTextItem*   linkName;
      InputLinkItem*       linkGraphic;
      InputLink*           link;
   };

   struct OutputData
   {
      OutputData();
      ~OutputData();
      void ResizeLinks(int count, EditorScene* scene);

      void Remove(EditorScene* scene);

      QGraphicsTextItem*   linkName;
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

      QGraphicsTextItem*   linkName;
      ValueLinkItem*       linkGraphic;
      std::vector<QGraphicsPathItem*> linkConnectors;
      ValueLink*           link;
   };

   /**
    * Draws a node in the graphics view.
    */
   class NodeItem : public QGraphicsPolygonItem
   {
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
      virtual void SetTitle(const std::string text);

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
      QColor GetColorForType(unsigned char type);
      QColor GetDarkColorForType(unsigned char type);

      /**
       * Retrieves the node.
       */
      Node* GetNode() {return mNode.get();}

      /**
       * Retrieves the ID of the node.
       */
      virtual dtCore::UniqueId GetNodeID();

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
       * Connects an output to an input.
       *
       * @param[in]  output  The output.
       * @param[in]  input   The input.
       * @param[in]  index   The output link index.
       */
      void ConnectLinks(OutputData& output, InputData& input, int index);

      /**
       * Connects a value link to a value node.
       *
       * @param[in]  output  The value link data.
       * @param[in]  input   The node item.
       * @param[in]  index   The value link index.
       */
      void ConnectLinks(ValueData& output, NodeItem* input, int index);

   protected:

      EditorScene* mScene;
      bool         mLoading;

      QPolygonF   mPolygon;
      QMenu*      mContextMenu;

      QGraphicsTextItem* mTitle;
      QGraphicsRectItem* mTitleBG;

      QGraphicsTextItem* mComment;

      dtCore::RefPtr<Node> mNode;

      std::vector<InputData>  mInputs;
      std::vector<OutputData> mOutputs;
      std::vector<ValueData>  mValues;

      QGraphicsRectItem* mLinkDivider;
      QGraphicsRectItem* mValueDivider;

      int         mNodeWidth;
      int         mNodeHeight;

      float       mTextHeight;

      float       mLinkWidth;
      float       mLinkHeight;
   };
}

#endif
