/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
* Chris Rodgers
*/

#ifndef DELTA_NODE_GRAPH_VIEW_H
#define DELTA_NODE_GRAPH_VIEW_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVE
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsItem>
#include <osg/Node>
#include <dtCore/refptr.h>



namespace Ui
{
   class NodeGraphViewer;
}



namespace dtQt
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   /*class DT_QT_EXPORT BaseNodeWrapper : public osg::Referenced
   {
   public:
      BaseNodeWrapper() {}

      virtual void SetParentNode(BaseNodeWrapper* node) = 0;
      virtual dtCore::RefPtr<BaseNodeWrapper> GetParentNode() const = 0;

   protected:
      virtual ~BaseNodeWrapper() {}
   };

   typedef dtCore::RefPtr<BaseNodeWrapper> BaseNodeWrapperPtr;

   template<class T_Node>
   class NodeWrapperT : public BaseNodeWrapper
   {
   public:
      typedef BaseNodeWrapper BaseClass;

      NodeWrapperT() {}

   protected:
      virtual ~NodeWrapperT() {}
   };*/



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeSigSlotInterface : public QObject
   {
      Q_OBJECT
   public:
   signals:
      void SignalMoved();
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class NodeItem;
   typedef std::vector<NodeItem*> NodeItemArray;

   class DT_QT_EXPORT NodeItem : public QGraphicsItem, virtual public NodeSigSlotInterface
   {
   public:
      typedef QGraphicsItem BaseClass;

      enum { Type = UserType + 1 };

      NodeItem(osg::Node& node);
      virtual ~NodeItem();

      // Enable the use of qgraphicsitem_cast with this item.
      int type() const;

      void SetCollapsed(bool collapsed);
      bool IsCollapsed() const;

      NodeItemArray CreateChildNodeItems(bool recurse);

      static NodeItem* ConvertToNodeItem(QGraphicsItem& item);

      NodeItem* GetParentNodeItem() const;
      unsigned int GetChildNodeItems(NodeItemArray& outChildren) const;

      QPointF GetCenter() const;

      float GetWidth() const;
      float GetHeight() const;

      QRectF boundingRect() const override;

      void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;


   protected:
      void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
      void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
      void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) override;

      bool mCollapsed;
      dtCore::RefPtr<osg::Node> mNode;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ConnectorSigSlotInterface : public QObject
   {
      Q_OBJECT
   public slots:
      virtual void OnNodeMoved() {}
   };

   class NodeConnector : public QGraphicsItem, virtual public ConnectorSigSlotInterface
   {
   public:
      typedef QGraphicsItem BaseClass;

      NodeConnector(NodeItem& nodeA, NodeItem& nodeB);

      virtual ~NodeConnector();

      void Set(NodeItem& nodeA, NodeItem& nodeB);

      QRectF boundingRect() const override;

      void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

      void Update();

      virtual void OnNodeMoved() override;

   protected:
      void PaintEnd(QPainter& painter, const QPointF& point, const QPointF& dims, const QPointF& direction);

      void UpdateNodeConnectors();

      NodeItem* mNodeA;
      NodeItem* mNodeB;
   };

   typedef std::vector<NodeConnector*> NodeConnectorArray;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeConnectorManager : public osg::Referenced
   {
   public:
      NodeConnectorManager();

      NodeConnectorArray CreateConnectors(NodeItem& node, bool recurse);

   protected:
      virtual ~NodeConnectorManager();
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeArranger : public osg::Referenced
   {
   public:
      struct Params
      {
         float mPaddingH;
         float mPaddingV;

         Params(float paddingH = 10.0f, float paddingV = 10.0f)
            : mPaddingH(paddingH)
            , mPaddingV(paddingV)
         {}
      };

      NodeArranger() {}

      NodeItem* FindDeepestNode(NodeItem& node);

      QRectF Arrange(NodeItem& node, const Params& params);

   protected:
      QRectF Arrange_Internal(NodeItem& node, const Params& params);

      virtual ~NodeArranger() {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeGraphScene : public QGraphicsScene
   {
      Q_OBJECT
   public:
      NodeGraphScene();
      virtual ~NodeGraphScene();

      void SetSceneNode(osg::Node* node);

      void UpdateScene();

   public slots:
      void OnSelectionChanged();

   //signals:
      //void SignalNodesSelected(???);

   protected:
      typedef std::list<NodeConnector*> NodeConnectorList;
      NodeConnectorList mConnectors;

      dtCore::RefPtr<osg::Node> mScene;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeGraphView : public QGraphicsView
   {
      Q_OBJECT
   public:
      typedef QGraphicsView BaseClass;

      NodeGraphView();
      virtual ~NodeGraphView();

      NodeGraphScene* GetNodeGraphScene();
      const NodeGraphScene* GetNodeGraphScene() const;

      void SetZoom(float zoom);
      float GetZoom() const;

      void SetZoomMin(float zoom);
      float GetZoomMin() const;

      void SetZoomMax(float zoom);
      float GetZoomMax() const;

      virtual void mousePressEvent(QMouseEvent* mouseEvent) override;
      virtual void mouseReleaseEvent(QMouseEvent* mouseEvent) override;
      virtual void mouseMoveEvent(QMouseEvent* mouseEvent) override;
      virtual void wheelEvent(QWheelEvent* wheelEvent) override;

   protected:
      NodeGraphScene* mNodeGraphScene;
      float mZoom;
      float mZoomMin;
      float mZoomMax;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeGraphViewerPanel : public QWidget
   {
   public:
      NodeGraphViewerPanel();
      virtual ~NodeGraphViewerPanel();

      NodeGraphView& GetGraphView();
      const NodeGraphView& GetGraphView() const;

      void UpdateUI();

   protected:
      void CreateConnections();

   private:
      Ui::NodeGraphViewer* mUI;
      NodeGraphView* mGraphView;
   };

} // END - namespace dtQt

#endif
