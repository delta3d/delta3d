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
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QWidget>
#include <QtGui/QGraphicsSceneContextMenuEvent>
#include <QtGui/QPen>

#include <dtDirector/node.h>


#define LINK_SIZE    12
#define LINK_LENGTH  15
#define LINK_PADDING 5

#define MIN_VALUE_NAME_SIZE 80
#define MAX_VALUE_NAME_SIZE 120

#define MIN_NODE_WIDTH  60
#define MIN_NODE_HEIGHT 50

#define MAX_NODE_WIDTH  500

class QGraphicsColorizeEffect;

namespace dtDirector
{
   class EditorScene;
   class InputLinkItem;
   class OutputLinkItem;
   class ValueLinkItem;
   class ValueNodeLinkItem;
   class NodeItem;

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

      void DrawGlow(float glow);

      GraphicsTextItem*    linkName;
      InputLinkItem*       linkGraphic;
      InputLink*           link;
      NodeItem*            node;
   };

   struct OutputData
   {
      OutputData();
      ~OutputData();
      void ResizeLinks(int count, EditorScene* scene);

      void Remove(EditorScene* scene);

      void DrawGlow(float glow);

      GraphicsTextItem*    linkName;
      OutputLinkItem*      linkGraphic;
      std::vector<QGraphicsPathItem*> linkConnectors;
      OutputLink*          link;
      NodeItem*            node;
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
      NodeItem*            node;
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
       * @param[in]  node      The Director Node.
       * @param[in]  readOnly  True if this node is read only.
       * @param[in]  imported  True if this node is an imported node.
       * @param[in]  parent    The parent item.
       * @param[in]  scene     The scene.
       */
      NodeItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Destructor.
       */
      virtual ~NodeItem();

      /**
       *	Retrieves whether this node is editable.
       */
      virtual bool IsEditable() const {return !mIsReadOnly;}

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
       * @param[in]  text   The new comment text.
       */
      void SetComment(const std::string& text);

      /**
       * Finds all of the links.
       */
      virtual void FindLinks();

      /**
       * Draws the title of the node.
       */
      virtual void DrawTitle();

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
       * Draws the comment.
       */
      virtual void DrawComment();

      /**
       * Draws a glow around the node.
       */
      virtual void DrawGlow();

      /**
       * Retrieves the node.
       *
       * @return  The node.
       */
      Node* GetNode() const {return mNode.get();}

      /**
       * Retrieves the macro (if available).
       *
       * @return  The macro.
       */
      virtual DirectorGraph* GetMacro() const {return NULL;}

      /**
       * Retrieves whether the node is contained.
       *
       * @param[in]  node  The node.
       *
       * @return true if the node is contained.
       */
      virtual bool HasNode(Node* node);

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
      virtual dtDirector::ID GetID();

      /**
       * Checks if this item contains a node of the given ID.
       *
       * @param[in]  id  The ID to check.
       */
      virtual bool HasID(const dtDirector::ID& id);

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
       * @param[in]  start                    The starting position.
       * @param[in]  end                      The ending position.
       * @param[in]  ownerNode                The owner that the line belongs to.
       * @param[in]  targetNode               The target this line is connecting to, If any.
       * @param[in]  height                   The height of the node.
       * @param[in]  drawReverse              Draws the link in reverse.
       * @param[in]  connectingTwoValueLinks  True if we are connecting two value links together.
       *
       * @return     The resulting painter path.
       */
      QPainterPath CreateConnectionH(QPointF start, QPointF end, NodeItem* ownerNode, NodeItem* targetNode, bool drawReverse = false, bool building = false);
      QPainterPath CreateConnectionV(QPointF start, QPointF end, bool drawReverse = false, bool connectingTwoValueLinks = false);

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

      /**
       * get value link color.
       */
      virtual QColor GetValueLinkColor(ValueLink* link) const;

      /**
       *	Retrieves whether this item is referencing an imported node
       */
      bool IsImported() const {return mIsImported;}

      /**
       * Retrieves whether this item is read only.
       */
      bool IsReadOnly() const {return mIsReadOnly;}


      static const QColor LINE_COLOR;

   public slots:

      /**
       * Event handler when a value link is being exposed.
       */
      virtual void ExposeLink(QAction* action);

      /**
       * Event handler when the trigger event has been clicked.
       */
      void OnEventTriggered(QAction* action);

      /**
       * Event handler when the toggle breakpoint has been clicked.
       */
      void OnToggleBreakPoint();

   protected:

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
      void SetBackgroundGradient();

      /**
       *	Set a dark foreground pen.
       */
      void SetDefaultPen();

      /**
       * Sets the amount of highlight applied to the current nodes pen.
       */
      void SetHighlight(float alpha);

      /**
       * Retrieves a menu for exposing links.
       */
      virtual QMenu* GetExposeLinkMenu();

      /**
       * Retrieves a menu for changing the visibility of links.
       */
      QMenu* GetLinkVisibilityMenu();
      QMenu* GetShowInputMenu();
      QMenu* GetShowOutputMenu();
      QMenu* GetShowValueMenu();

   protected:

      EditorScene* mScene;
      bool         mLoading;
      bool         mIsReadOnly;
      bool         mIsImported;

      std::string  mOldPosition;

      QPolygonF   mPolygon;

      GraphicsTextItem* mTitle;
      QGraphicsRectItem* mTitleBG;
      QGraphicsPixmapItem* mLatentIcon;

      GraphicsTextItem* mComment;

      dtCore::ObserverPtr<Node> mNode;

      std::vector<InputData>  mInputs;
      std::vector<OutputData> mOutputs;
      std::vector<ValueData>  mValues;

      QGraphicsRectItem* mLinkDivider;
      QGraphicsRectItem* mValueDivider;
      QGraphicsRectItem* mTitleDivider;

      QPen        mNodePen;
      QColor      mPenColor;

      int         mNodeWidth;
      int         mNodeHeight;

      float       mTitleHeight;

      float       mLinkWidth;
      float       mLinkHeight;

      float       mValueHeight;

      bool        mHasHiddenLinks;

      bool        mChainSelecting;

   };
}

#endif
