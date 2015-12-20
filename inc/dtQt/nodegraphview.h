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
#include <dtGame/gameactorproxy.h>



namespace Ui
{
   class NodeGraphViewer;
}



namespace dtQt
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class BaseNodeWrapper;
   typedef dtCore::RefPtr<BaseNodeWrapper> BaseNodeWrapperPtr;
   typedef std::vector<BaseNodeWrapperPtr> BaseNodeWrapperArray;

   class DT_QT_EXPORT BaseNodeWrapper : public osg::Referenced
   {
   public:
      typedef dtCore::RefPtr<BaseNodeWrapper> Ptr;

      BaseNodeWrapper() {}

      virtual std::string GetName() const = 0;
      virtual std::string GetClassName() const = 0;
      virtual std::string GetDescription() const = 0;
      virtual void SetParentNode(BaseNodeWrapper* node) = 0;
      virtual BaseNodeWrapperPtr GetParentNode() const = 0;
      virtual unsigned int GetChildNodes(BaseNodeWrapperArray& outArray) = 0;

      template<class T_DerivedType>
      T_DerivedType* As()
      {
         return dynamic_cast<T_DerivedType*>(this);
      }

      template<class T_DerivedType>
      const T_DerivedType* As() const
      {
         return dynamic_cast<const T_DerivedType*>(this);
      }

   protected:
      virtual ~BaseNodeWrapper() {}
   };



   template<class T_Node, class T_Ptr>
   class NodeWrapperT : public BaseNodeWrapper
   {
   public:
      typedef BaseNodeWrapper BaseClass;
      typedef T_Node Type;
      typedef T_Ptr TypePtr;

      NodeWrapperT(Type& node)
         : mObj(&node)
      {}

      Type* Get() const
      {
         return mObj;
      }

      Type& operator* ()
      {
         return *mObj;
      }

   protected:
      virtual ~NodeWrapperT() {}

      TypePtr mObj;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   typedef NodeWrapperT<osg::Node, dtCore::RefPtr<osg::Node> > BaseOsgNodeWrapper;
   class DT_QT_EXPORT OsgNodeWrapper : public BaseOsgNodeWrapper
   {
   public:
      typedef BaseOsgNodeWrapper BaseClass;
      
      OsgNodeWrapper(osg::Node& node);

      std::string GetName() const override;

      std::string GetClassName() const override;

      std::string GetDescription() const override;

      void SetParentNode(BaseNodeWrapper* nodeWrapper) override;

      unsigned int GetChildNodes(BaseNodeWrapperArray& outArray) override;

      BaseNodeWrapperPtr GetParentNode() const override;

   protected:
      virtual ~OsgNodeWrapper();
   };

   typedef dtCore::RefPtr<OsgNodeWrapper> OsgNodeWrapperPtr;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   typedef NodeWrapperT<dtCore::BaseActorObject, dtCore::RefPtr<dtCore::BaseActorObject> > BaseActorNodeWrapper;
   class DT_QT_EXPORT ActorNodeWrapper : public BaseActorNodeWrapper
   {
   public:
      typedef BaseActorNodeWrapper BaseClass;

      ActorNodeWrapper(dtCore::BaseActorObject& actor);

      std::string GetName() const override;

      std::string GetClassName() const override;

      std::string GetDescription() const override;

      void SetParentNode(BaseNodeWrapper* nodeWrapper) override;

      unsigned int GetChildNodes(BaseNodeWrapperArray& outArray) override;

      BaseNodeWrapperPtr GetParentNode() const override;

      dtGame::GameActorProxy* GetAsActor() const;

   protected:
      virtual ~ActorNodeWrapper();
   };

   typedef dtCore::RefPtr<ActorNodeWrapper> ActorNodeWrapperPtr;



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

      NodeItem(BaseNodeWrapper& node);
      virtual ~NodeItem();

      // Enable the use of qgraphicsitem_cast with this item.
      int type() const;

      void SetCollapsed(bool collapsed);
      bool IsCollapsed() const;

      NodeItemArray CreateChildNodeItems(bool recurse);

      static NodeItem* ConvertToNodeItem(QGraphicsItem& item);

      BaseNodeWrapper& GetNodeWrapper();
      const BaseNodeWrapper& GetNodeWrapper() const;

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
      dtCore::RefPtr<BaseNodeWrapper> mNode;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ConnectorSigSlotInterface : public QObject
   {
      Q_OBJECT
   public slots:
      virtual void OnNodeMoved() = 0;
   };

   class NodeConnector : public QGraphicsItem, virtual public ConnectorSigSlotInterface
   {
   public:
      typedef QGraphicsItem BaseClass;

      NodeConnector(NodeItem& nodeA, NodeItem& nodeB);

      virtual ~NodeConnector();

      void Set(NodeItem& nodeA, NodeItem& nodeB);

      NodeItem* GetNodeA() const;
      NodeItem* GetNodeB() const;

      bool HasNode(const NodeItem& node) const;

      bool IsValid() const;

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
   typedef std::list<NodeConnector*> NodeConnectorList;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeConnectorManager : public osg::Referenced
   {
   public:
      NodeConnectorManager();

      NodeConnector* CreateConnector(NodeItem& nodeParent, NodeItem& nodeChild);

      NodeConnectorArray CreateConnectors(NodeItem& node, bool recurse);

      void RemoveConnectorToParent(NodeItem& node);

      NodeConnectorList& GetConnectors();
      const NodeConnectorList& GetConnectors() const;

      int GetConnectorsForNode(const NodeItem& node, NodeConnectorList& outConnectors) const;

   protected:
      virtual ~NodeConnectorManager();

      NodeConnectorList mConnectors;
   };

   typedef dtCore::RefPtr<NodeConnectorManager> NodeConnectorManagerPtr;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeArranger : public osg::Referenced
   {
   public:
      static const float DEFAULT_PADDING_H;
      static const float DEFAULT_PADDING_V;

      struct Params
      {
         float mPaddingH;
         float mPaddingV;

         Params(float paddingH = DEFAULT_PADDING_H,
            float paddingV = DEFAULT_PADDING_V)
            : mPaddingH(paddingH)
            , mPaddingV(paddingV)
         {}
      };

      NodeArranger() {}

      QRectF Arrange(NodeItem& node, const Params& params);

   protected:
      QRectF Arrange_Internal(NodeItem& node, const Params& params);

      virtual ~NodeArranger() {}
   };

   typedef dtCore::RefPtr<NodeArranger> NodeArrangerPtr;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeGraphScene : public QGraphicsScene
   {
      Q_OBJECT
   public:
      NodeGraphScene();
      virtual ~NodeGraphScene();

      void SetSceneNodes(const BaseNodeWrapperArray& nodes);

      /*
      * Acquires the objects referenced by the currently selected nodes.
      * @param outObjects Container to capture the objects of the selected nodes.
      * @return Count of objects retrieved.
      */
      unsigned int GetSelectedNodes(BaseNodeWrapperArray& outNodes);

      /*
      * Acquires the currently selected nodes.
      * @param outNodeItems Container to capture the selected nodes.
      * @return Count of nodes retrieved.
      */
      unsigned int GetSelectedNodeItems(NodeItemArray& outNodeItems);

      void GetNodesFromItems(const NodeItemArray& nodeItems, BaseNodeWrapperArray& outNodes);

      void UpdateScene();

      /*
      * Method to detach graphical nodes from their graphical node parents.
      * NOTE: This is only for graphics does not affect the actual
      * relationships between objects represented by the nodes.
      */
      void DetachNodeItems(const NodeItemArray& nodeItems);

      /*
      * Method to attach graphical nodes to a graphical node parent.
      * NOTE: This is only for graphics does not affect the actual
      * relationships between objects represented by the nodes.
      */
      void AttachNodeItems(const NodeItemArray& nodeItems, NodeItem& parentNodeItem);

      void DetachSelectedNodes();

      void AttachSelectedNodes(NodeItem& parentNodeItem);

      int GetNodeConnectorsForNode(const NodeItem& nodeItem, NodeConnectorList& outConnectors) const;

   public slots:
      void OnSelectionChanged();
      void OnDetachAction();

   signals:
      void SignalNodesSelected(const dtQt::BaseNodeWrapperArray& nodes);
      void SignalNodesDetached(const dtQt::BaseNodeWrapperArray& nodes);
      void SignalNodesAttached(const dtQt::BaseNodeWrapperArray& nodes, const BaseNodeWrapper& parentNode);

   private:
      void CreateConnections();

      NodeConnectorManagerPtr mConnectorManager;
      BaseNodeWrapperArray mSceneNodes;
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

   signals:
      void SignalZoomChanged(float zoom);

   public slots:
      void OnZoomAction(const QString zoomValue);

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
      Q_OBJECT
   public:
      NodeGraphViewerPanel();
      virtual ~NodeGraphViewerPanel();

      NodeGraphView& GetNodeGraphView();
      const NodeGraphView& GetNodeGraphView() const;

      void UpdateUI();

   public slots:
      void OnZoomChanged(float value);

   protected:
      void CreateConnections();

   private:
      Ui::NodeGraphViewer* mUI;
      NodeGraphView* mGraphView;
   };

} // END - namespace dtQt



////////////////////////////////////////////////////////////////////////////////
// QT TYPES FOR SIGNALS
////////////////////////////////////////////////////////////////////////////////
Q_DECLARE_METATYPE(dtQt::BaseNodeWrapperArray);

#endif
