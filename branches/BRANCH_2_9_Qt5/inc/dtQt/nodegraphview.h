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
      BaseNodeWrapper() {}

      virtual std::string GetName() const = 0;
      virtual std::string GetClassName() const = 0;
      virtual std::string GetDescription() const = 0;

      virtual const dtCore::UniqueId& GetId() const = 0;

      virtual void SetParentNode(BaseNodeWrapper* node) = 0;
      virtual BaseNodeWrapperPtr GetParentNode() const = 0;

      virtual unsigned int GetChildNodes(BaseNodeWrapperArray& outArray) = 0;

      virtual bool Equals(const BaseNodeWrapper& other) const = 0;

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

      bool Equals(const BaseNodeWrapper& other) const override
      {
         return GetId() == other.GetId();
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

      const dtCore::UniqueId& GetId() const override;

      void SetParentNode(BaseNodeWrapper* nodeWrapper) override;

      unsigned int GetChildNodes(BaseNodeWrapperArray& outArray) override;

      BaseNodeWrapperPtr GetParentNode() const override;

   protected:
      virtual ~OsgNodeWrapper();

      dtCore::UniqueId mId;
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

      const dtCore::UniqueId& GetId() const override;

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

      void SetMovable(bool movable);
      bool IsMovable() const;

      void SetSelectable(bool selectable);
      bool IsSelectable() const;

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

      void SetAbsoluteMode(bool absoluteMode);
      bool IsAbsoluteMode() const;

      bool HasNode(const NodeItem& node) const;

      bool IsValid() const;

      QRectF boundingRect() const override;

      void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

      void Update();

      virtual void OnNodeMoved() override;

   protected:
      void PaintEnd(QPainter& painter, const QPointF& point, const QPointF& dims, const QPointF& direction);

      void UpdateNodeConnectors();

      bool mAbsoluteSpace;
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

      NodeConnector* CreateConnector(NodeItem& nodeParent, NodeItem& nodeChild, bool makeConnectorChild = true);

      NodeConnectorArray CreateConnectors(NodeItem& nodeParent, const NodeItemArray& childNodes, bool makeConnectorChild = true);

      NodeConnectorArray CreateConnectorsToChildren(NodeItem& node, bool recurse);

      void RemoveConnectorToParent(NodeItem& node);

      NodeConnectorList& GetConnectors();
      const NodeConnectorList& GetConnectors() const;

      int GetConnectorsForNode(const NodeItem& node, NodeConnectorList& outConnectors) const;

      unsigned int Clear();

      void UpdateConnectors();

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
   typedef dtUtil::Functor<void, TYPELIST_1(BaseNodeWrapperArray&)> NodeProviderFunc;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeGraphScene : public QGraphicsScene
   {
      Q_OBJECT
   public:
      NodeGraphScene();
      virtual ~NodeGraphScene();

      /*
      * Adds a single node to the scene.
      * @param node The node to add to the scene.
      * @param addChildren Determines if the method should also add all descendant nodes of the specified node.
      * @return a new node item if the node was added, or NULL if the node already exists.
      */
      NodeItem* AddNode(BaseNodeWrapper& node, bool addChildren);

      /*
      * Adds several nodes to the scene.
      * @param nodes The collection of nodes to add to the scene.
      * @param addChildren Determines if the method should also add all descendant nodes of a specified node.
      * @param outNodeItems Optional container for capturing all created node items for the scene.
      * @return Count of nodes that were created and added to the scene.
      */
      unsigned int AddNodes(const BaseNodeWrapperArray& nodes, bool addChildren, NodeItemArray* outNodeItems = nullptr);

      bool RemoveNode(BaseNodeWrapper& node);
      bool RemoveNodeItem(NodeItem& nodeItem);

      void SetNodeProviderFunc(NodeProviderFunc nodeProviderFunc);

      NodeItem* FindNodeItem(BaseNodeWrapper& node) const;

      unsigned int SetNodesSelected(const BaseNodeWrapperArray& nodes, bool selected);

      unsigned int SetSelectedNodes(const BaseNodeWrapperArray& nodes);

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

      /*
      * Refreshes the node graph scene by clearing all graphics items and reconstructing
      * the scene calling upon the specified node provider functor. The overload version
      * of this method that does not take a functor will use the functor set for this
      * scene via SetNodeProviderFunc.
      */
      void UpdateScene(NodeProviderFunc nodeProviderFunc);
      void UpdateScene();

      /*
      * Method to detach graphical nodes from their graphical node parents.
      * NOTE: This is only for graphics and does not affect the actual
      * relationships between objects represented by the node items.
      */
      void DetachNodeItem(NodeItem& nodeItem);
      void DetachNodeItems(const NodeItemArray& nodeItems);

      /*
      * Method to attach graphical nodes to a graphical node parent.
      * NOTE: This is only for graphics and does not affect the actual
      * relationships between objects represented by the node items.
      */
      void AttachNodeItem(NodeItem& nodeItem, NodeItem& parentNodeItem);
      void AttachNodeItems(const NodeItemArray& nodeItems, NodeItem& parentNodeItem);

      void DetachSelectedNodes();

      void AttachSelectedNodes(NodeItem& parentNodeItem);

      /**
       * Updates the floating connector position, the
       */
      void SetFloatingConnectorsEndPosition(const QPointF& pos);
      void CreateFloatingConnectorsToNodeItems(const NodeItemArray& nodeItems);

      void UpdateFloatingConnectors();

      void ClearFloatingConnectors();

      bool IsAttachEnabled() const;

      void ClearNodeItems();

   public slots:
      void OnSelectionChanged();
      void OnDetachAction();
      void OnAttachAction(bool attachEnabled);

   signals:
      void SignalNodesSelected(const dtQt::BaseNodeWrapperArray& nodes);
      void SignalNodesDetached(const dtQt::BaseNodeWrapperArray& nodes);
      void SignalNodesAttached(const dtQt::BaseNodeWrapperArray& nodes, const dtQt::BaseNodeWrapper& parentNode);

      void SignalNodeAttached(const dtQt::BaseNodeWrapper& node, const dtQt::BaseNodeWrapper& parentNode);
      void SignalNodeDetached(const dtQt::BaseNodeWrapper& node);

      void SignalNodeItemAdded(const dtQt::NodeItem& nodeItem);
      void SignalNodeItemRemoved(const dtQt::NodeItem& nodeItem);

      void SignalCleared();

   private:
      void CreateConnections();

      NodeConnectorManagerPtr mConnectorManager;
      NodeConnectorManagerPtr mFloatingConnectorManager;
      NodeItem* mFloatNode;

      bool mAttachMode;

      typedef dtUtil::RefString IdType;
      typedef std::map<IdType, NodeItem*> IdNodeItemMap;
      IdNodeItemMap mIdItemMap;

      NodeProviderFunc mNodeProviderFunc;
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

      NodeItem* GetNodeItemAt(const QPoint& scenePos) const;

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
      void SignalNodeItemOnMouseClick(NodeItem& nodeItem);
      void SignalNodeItemOnMouseRelease(NodeItem* nodeItem);

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
