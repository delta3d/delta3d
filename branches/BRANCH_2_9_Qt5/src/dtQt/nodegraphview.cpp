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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_nodegraphviewer.h"
#include <sstream>
#include <dtQt/nodegraphview.h>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainterPath>
#include <osg/Group>
#include <dtQt/constants.h>
#include <dtUtil/mathdefines.h>



namespace dtQt
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   OsgNodeWrapper::OsgNodeWrapper(osg::Node& node)
      : BaseClass(node)
      , mId(node.getName() + "(" + mObj->className() + ")") // TODO: Ensure node id value is unique
   {}

   OsgNodeWrapper::~OsgNodeWrapper()
   {}

   std::string OsgNodeWrapper::GetName() const
   {
      return mObj->getName();
   }

   std::string OsgNodeWrapper::GetClassName() const
   {
      return mObj->className();
   }

   std::string OsgNodeWrapper::GetDescription() const
   {
      std::ostringstream oss;
      typedef osg::Node::DescriptionList DescList;
      DescList descList = mObj->getDescriptions();

      size_t i = 0;
      size_t limit = descList.size();
      std::for_each(descList.begin(), descList.end(),
         [&](const std::string& str)
      {
         oss << str;
         if (i + 1 < limit)
         {
            oss << "\n";
         }
      }
      );

      return oss.str().c_str();
   }

   const dtCore::UniqueId& OsgNodeWrapper::GetId() const
   {
      return mId;
   }

   void OsgNodeWrapper::SetParentNode(BaseNodeWrapper* nodeWrapper)
   {
      OsgNodeWrapper* parentWrapper = dynamic_cast<OsgNodeWrapper*>(nodeWrapper);

      if (parentWrapper != nullptr)
      {
         osg::Group* parent = dynamic_cast<osg::Group*>(parentWrapper->Get());

         if (parent != nullptr)
         {
            // TODO: Attach using action object...
            parent->addChild(mObj.get());
         }
         else
         {
            // TODO: Detach using action object...
            while (mObj->getNumParents() > 0)
            {
               mObj->getParent(0)->removeChild(mObj.get());
            }
         }
      }
   }

   BaseNodeWrapperPtr OsgNodeWrapper::GetParentNode() const
   {
      BaseNodeWrapperPtr nodeWrapper;
      osg::Group* parent = mObj->getNumParents() > 0 ? mObj->getParent(0) : nullptr;

      if (parent != nullptr)
      {
         nodeWrapper = new OsgNodeWrapper(*parent);
      }

      return nodeWrapper;
   }

   unsigned int OsgNodeWrapper::GetChildNodes(BaseNodeWrapperArray& outArray)
   {
      unsigned int addCount = 0;

      osg::Group* group = mObj->asGroup();

      if (group != nullptr)
      {
         unsigned int numChildren = group->getNumChildren();
         outArray.reserve(numChildren);

         NodeItem* curNodeItem = nullptr;
         osg::Node* curNode = nullptr;
         for (unsigned int i = 0; i < numChildren; ++i)
         {
            curNode = group->getChild(i);

            outArray.push_back(new OsgNodeWrapper(*curNode));

            ++addCount;
         }
      }

      return addCount;
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   ActorNodeWrapper::ActorNodeWrapper(dtCore::BaseActorObject& actor)
      : BaseClass(actor)
   {}

   ActorNodeWrapper::~ActorNodeWrapper()
   {}

   std::string ActorNodeWrapper::GetName() const
   {
      return mObj->GetName();
   }

   std::string ActorNodeWrapper::GetClassName() const
   {
      return mObj->GetActorType().GetName();
   }

   std::string ActorNodeWrapper::GetDescription() const
   {
      return mObj->GetActorType().GetDescription();
   }

   const dtCore::UniqueId& ActorNodeWrapper::GetId() const
   {
      return mObj->GetId();
   }

   void ActorNodeWrapper::SetParentNode(BaseNodeWrapper* nodeWrapper)
   {
      ActorNodeWrapper* parentNodeWrapper = dynamic_cast<ActorNodeWrapper*>(nodeWrapper);

      dtGame::GameActorProxy* gameActor = GetAsActor();

      if (gameActor != nullptr)
      {
         gameActor->SetParentActor(parentNodeWrapper == nullptr ? nullptr : parentNodeWrapper->GetAsActor());
      }
   }

   unsigned int ActorNodeWrapper::GetChildNodes(BaseNodeWrapperArray& outArray)
   {
      unsigned int addCount = 0;

      dtGame::GameActorProxy* gameActor = GetAsActor();

      if (gameActor != nullptr)
      {
         dtGame::GameActorProxy::child_iterator iter = gameActor->begin_child();
         dtGame::GameActorProxy::child_iterator endIter = gameActor->end_child();

         BaseClass::Type* child = nullptr;
         for (; iter != endIter; ++iter)
         {
            child = (&(*iter));

            if (child == nullptr)
            {
               // TODO: Log warning
            }
            else
            {
               outArray.push_back(new ActorNodeWrapper(*child));

               ++addCount;
            }
         }
      }

      return addCount;
   }

   BaseNodeWrapperPtr ActorNodeWrapper::GetParentNode() const
   {
      BaseNodeWrapperPtr nodeWrapper;

      dtGame::GameActorProxy* gameActor = GetAsActor();

      if (gameActor != nullptr)
      {
         BaseClass::Type* parent = gameActor->GetParentActor();

         if (parent != nullptr)
         {
            nodeWrapper = new ActorNodeWrapper(*parent);
         }
      }

      return nodeWrapper;
   }

   dtGame::GameActorProxy* ActorNodeWrapper::GetAsActor() const
   {
      return dynamic_cast<dtGame::GameActorProxy*>(mObj.get());
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeItem::NodeItem(BaseNodeWrapper& node)
      : mCollapsed(false)
      , mNode(&node)
   {
      setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

      setZValue(1.0f);

      setToolTip(node.GetDescription().c_str());
   }

   NodeItem::~NodeItem()
   {}

   int NodeItem::type() const
   {
      return NodeItem::Type;
   }

   void NodeItem::SetMovable(bool movable)
   {
      if (movable)
      {
         setFlags(flags() | QGraphicsItem::ItemIsMovable);
      }
      else
      {
         setFlags(flags() & (~QGraphicsItem::ItemIsMovable));
      }
   }

   bool NodeItem::IsMovable() const
   {
      return (flags() | QGraphicsItem::ItemIsMovable) > 0;
   }

   void NodeItem::SetSelectable(bool movable)
   {
      if (movable)
      {
         setFlags(flags() | QGraphicsItem::ItemIsSelectable);
      }
      else
      {
         setFlags(flags() & (~QGraphicsItem::ItemIsSelectable));
      }
   }

   bool NodeItem::IsSelectable() const
   {
      return (flags() | QGraphicsItem::ItemIsSelectable) > 0;
   }

   void NodeItem::SetCollapsed(bool collapsed)
   {
      mCollapsed = collapsed;
   }

   bool NodeItem::IsCollapsed() const
   {
      return mCollapsed;
   }

   NodeItemArray NodeItem::CreateChildNodeItems(bool recurse)
   {
      NodeItemArray nodeItemArray;

      BaseNodeWrapperArray children;
      mNode->GetChildNodes(children);
   
      if ( ! children.empty())
      {
         nodeItemArray.reserve(children.size());

         NodeItem* curNodeItem = nullptr;
         std::for_each(children.begin(), children.end(),
            [&](BaseNodeWrapper* child)
            {
               curNodeItem = new NodeItem(*child);
               curNodeItem->setParentItem(this);
               nodeItemArray.push_back(curNodeItem);
            }
         );

         // Cascade creation down the node tree if recursion has been specified.
         if (recurse)
         {
            std::for_each(nodeItemArray.begin(), nodeItemArray.end(), 
               [&](NodeItem* item)
               {
                  item->CreateChildNodeItems(true);
               }
            );
         }
      }

      return nodeItemArray;
   }

   NodeItem* NodeItem::ConvertToNodeItem(QGraphicsItem& item)
   {
      return qgraphicsitem_cast<NodeItem*>(&item);
   }

   BaseNodeWrapper& NodeItem::GetNodeWrapper()
   {
      return *mNode;
   }

   const BaseNodeWrapper& NodeItem::GetNodeWrapper() const
   {
      return *mNode;
   }

   NodeItem* NodeItem::GetParentNodeItem() const
   {
      QGraphicsItem* parent = parentItem();

      return parent != nullptr ? ConvertToNodeItem(*parent) : nullptr;
   }

   unsigned int NodeItem::GetChildNodeItems(NodeItemArray& outChildren) const
   {
      NodeItemArray nodeItems;
      unsigned int count = 0;

      QList<QGraphicsItem*> children = childItems();

      std::for_each(children.begin(), children.end(),
         [&](QGraphicsItem* item)
         {
            NodeItem* nodeItem = ConvertToNodeItem(*item);

            if (nodeItem != nullptr)
            {
               outChildren.push_back(nodeItem);

               // Do not assume the collector container started out empty.
               // Track explicit count by a simple variable.
               ++count;
            }
         }
      );

      return count;
   }

   QPointF NodeItem::GetCenter() const
   {
      QPointF pt = boundingRect().center();
      return scenePos() + pt;
   }

   float NodeItem::GetWidth() const
   {
      return 200.0f;
   }

   float NodeItem::GetHeight() const
   {
      return 40.0f;
   }

   QRectF NodeItem::boundingRect() const
   {
      return QRectF(0.0, 0.0f, GetWidth() + 1.0f, GetHeight() + 1.0f);
   }

   void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
   {
      static const QColor SELECTED(Qt::red);
      static const QColor SELECTED_FILL(Qt::yellow);
      static const QColor NORMAL(Qt::black);
      static const QColor NORMAL_FILL(128, 128, 144);

      float w = GetWidth();
      float h = GetHeight();
      painter->setBrush(Qt::white);
      painter->drawRect(0.0f, 0.0f, h, h);

      QRectF textRect(h, 0.0f, w - h, h);
      painter->setPen(isSelected() ? SELECTED : NORMAL);
      painter->setBrush(isSelected() ? SELECTED_FILL : NORMAL_FILL);
      painter->drawRect(textRect);

      QTextOption op;
      op.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

      textRect.setX(textRect.x() + 5.0f);
      textRect.setWidth(textRect.width() - 5.0f);
      painter->setPen(isSelected() ? SELECTED : Qt::white);
      painter->drawText(textRect, mNode->GetName().c_str(), op);

      std::string nodeClass(mNode->GetClassName());
      const QString* iconPath = Constants::GetIconPathByClassName(nodeClass);

      QIcon icon;
      if (iconPath != nullptr)
      {
         icon = QIcon(*iconPath);
      }
      else
      {
         icon = QIcon(dtQt::Constants::ICON_GROUP);
      }

      painter->drawPixmap(3,3,icon.pixmap(QSize(40,40)));
   }

   void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      BaseClass::mousePressEvent(mouseEvent);
   }

   void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      BaseClass::mouseReleaseEvent(mouseEvent);
   }

   void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
   {
      BaseClass::mouseMoveEvent(mouseEvent);

      emit SignalMoved();
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeConnector::NodeConnector(NodeItem& nodeA, NodeItem& nodeB)
      : mAbsoluteSpace(false)
      , mNodeA(&nodeA)
      , mNodeB(&nodeB)
   {
      setFlags(QGraphicsItem::ItemStacksBehindParent);
   }

   NodeConnector::~NodeConnector()
   {}

   void NodeConnector::Set(NodeItem& nodeA, NodeItem& nodeB)
   {
      mNodeA = &nodeA;
      mNodeB = &nodeB;

      Update();
   }

   NodeItem* NodeConnector::GetNodeA() const
   {
      return mNodeA;
   }
   
   NodeItem* NodeConnector::GetNodeB() const
   {
      return mNodeB;
   }

   void NodeConnector::SetAbsoluteMode(bool absoluteMode)
   {
      mAbsoluteSpace = absoluteMode;
   }
   
   bool NodeConnector::IsAbsoluteMode() const
   {
      return mAbsoluteSpace;
   }

   bool NodeConnector::HasNode(const NodeItem& node) const
   {
      return mNodeA == &node || mNodeB == &node;
   }

   bool NodeConnector::IsValid() const
   {
      return mNodeA != nullptr && mNodeB != nullptr;
   }

   QRectF NodeConnector::boundingRect() const
   {
      QPointF pos = scenePos();
      QPointF ptA = mapToParent(mNodeA->GetCenter() - pos) + QPointF(0.0f, mNodeA->GetHeight() * 0.5f);
      QPointF ptB = mapToParent(mNodeB->GetCenter() - pos) - QPointF(0.0f, mNodeB->GetHeight() * 0.5f);

      if (mAbsoluteSpace)
      {
         ptA = mNodeA->GetCenter();
         ptB = mNodeB->GetCenter();
      }

      float penWidth = 10.0f;// 2.0f;

      QRectF rect(ptA, ptB);
      rect = rect.normalized();
      rect.setX(rect.x() - penWidth);
      rect.setY(rect.y() - penWidth);
      rect.setSize(rect.size() + QSizeF(penWidth, penWidth));
      return rect;
   }

   void NodeConnector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
   {
      painter->setRenderHint(QPainter::Antialiasing);

      QPointF endDims(10.0f, 10.0f);
      QPointF endOffset(0.0f, endDims.y());

      QPointF pos = scenePos();
      QPointF ptA = mapToParent(mNodeA->GetCenter() - pos) + QPointF(0.0f, mNodeA->GetHeight() * 0.5f) + endOffset;
      QPointF ptB = mapToParent(mNodeB->GetCenter() - pos) - QPointF(0.0f, mNodeB->GetHeight() * 0.5f);

      if (mAbsoluteSpace)
      {
         ptA = mNodeA->GetCenter();
         ptB = mNodeB->GetCenter();
      }

      QPointF mid = (ptB - ptA) * 0.5f + ptA;

      QPointF cpA(ptA.x(), mid.y());
      QPointF cpB(ptB.x(), mid.y());

      QColor color = Qt::black;
      QPen pen;
      pen.setColor(color);
      pen.setWidth(1);

      QPainterPath path;
      path.moveTo(ptA);
      path.cubicTo(cpA, cpB, ptB);

      painter->setPen(color);
      painter->drawPath(path);

      painter->setBrush(color);
      QPointF dir(0.0f, 1.0f);
      PaintEnd(*painter, ptA - endOffset, endDims, -dir);
      //PaintEnd(*painter, ptB, dims, dir);
   }

   void NodeConnector::PaintEnd(QPainter& painter, const QPointF& point, const QPointF& dims, const QPointF& direction)
   {
      float y = dims.y() * direction.y();

      painter.setBrush(mAbsoluteSpace ? Qt::green : Qt::red);

      QPolygonF poly;
      poly.push_back(point);
      poly.append(point + QPointF(-dims.x() * 0.5f, -y));
      poly.append(point + QPointF(dims.x() * 0.5f, -y));
      painter.drawPolygon(poly);
   }

   void NodeConnector::Update()
   {
      prepareGeometryChange();
   }

   void NodeConnector::OnNodeMoved()
   {
      Update();
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeConnectorManager::NodeConnectorManager()
   {}

   NodeConnectorManager::~NodeConnectorManager()
   {}

   NodeConnector* NodeConnectorManager::CreateConnector(NodeItem& nodeParent, NodeItem& nodeChild, bool makeConnectorChild)
   {
      NodeConnector* connector = new NodeConnector(nodeParent, nodeChild);

      if (makeConnectorChild)
      {
         connector->setParentItem(&nodeParent);
      }

      mConnectors.push_back(connector);

      QObject::connect(&nodeChild, SIGNAL(SignalMoved()),
         connector, SLOT(OnNodeMoved()));

      return connector;
   }

   NodeConnectorArray NodeConnectorManager::CreateConnectors(NodeItem& nodeParent, const NodeItemArray& childNodes, bool makeConnectorChild)
   {
      NodeConnectorArray connectorArray;

      connectorArray.reserve(childNodes.size());

      NodeConnector* connector = nullptr;
      std::for_each(childNodes.begin(), childNodes.end(),
         [&](NodeItem* childNode)
         {
            connector = CreateConnector(nodeParent, *childNode, makeConnectorChild);

            if (connector != nullptr)
            {
               connectorArray.push_back(connector);
               mConnectors.push_back(connector);
            }
         }
      );

      return connectorArray;
   }

   NodeConnectorArray NodeConnectorManager::CreateConnectorsToChildren(NodeItem& node, bool recurse)
   {
      NodeConnectorArray connectorArray;

      NodeItemArray children;
      node.GetChildNodeItems(children);
      connectorArray.reserve(children.size());

      NodeConnector* connector = nullptr;
      std::for_each(children.begin(), children.end(),
         [&](NodeItem* childNode)
         {
            connector = CreateConnector(node, *childNode, true);
            connectorArray.push_back(connector);
            mConnectors.push_back(connector);
         }
      );

      // Determine if creation of connectors should recurse through the node tree.
      if (recurse)
      {
         std::for_each(children.begin(), children.end(),
            [&](NodeItem* childNode)
            {
               CreateConnectorsToChildren(*childNode, true);
            }
         );
      }

      return connectorArray;
   }

   void NodeConnectorManager::RemoveConnectorToParent(NodeItem& node)
   {
      NodeConnector* connector = nullptr;
      NodeConnectorList::iterator iter = mConnectors.begin();
      NodeConnectorList::iterator endIter = mConnectors.end();
      for (; iter != endIter; )
      {
         connector = *iter;

         if (connector->GetNodeB() == &node)
         {
            NodeConnectorList::iterator iterToErase = iter;
            ++iter;

            QGraphicsScene* scene = connector->scene();
            if (scene != nullptr)
            {
               scene->removeItem(connector);
            }

            // The item should have a reference back to the scene.
            // Make sure the scene removes the connector.
            connector->setParentItem(nullptr);
            
            /*delete connector;
            connector = nullptr;*/

            mConnectors.erase(iterToErase);
         }
         else
         {
            ++iter;
         }
      }
   }

   NodeConnectorList& NodeConnectorManager::GetConnectors()
   {
      return mConnectors;
   }

   const NodeConnectorList& NodeConnectorManager::GetConnectors() const
   {
      return mConnectors;
   }

   int NodeConnectorManager::GetConnectorsForNode(const NodeItem& node, NodeConnectorList& outConnectors) const
   {
      int count = 0;

      std::for_each(mConnectors.begin(), mConnectors.end(),
         [&](NodeConnector* connector)
         {
            if (connector->HasNode(node))
            {
               outConnectors.push_back(connector);
               ++count;
            }
         }
      );

      return count;
   }
   
   unsigned int NodeConnectorManager::Clear()
   {
      unsigned int count = 0;

      std::for_each(mConnectors.begin(), mConnectors.end(),
         [&](NodeConnector* connector)
         {
            QGraphicsScene* scene = connector->scene();
            if (scene != nullptr)
            {
               scene->removeItem(connector);
            }

            // The item should have a reference back to the scene.
            // Make sure the scene removes the connector.
            connector->setParentItem(nullptr);

            /*delete connector;
            connector = nullptr;*/
         }
      );

      mConnectors.clear();

      return count;
   }

   void NodeConnectorManager::UpdateConnectors()
   {
      std::for_each(mConnectors.begin(), mConnectors.end(),
         [&](NodeConnector* connector)
         {
            connector->Update();
         }
      );
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   const float NodeArranger::DEFAULT_PADDING_H = 10.0f;
   const float NodeArranger::DEFAULT_PADDING_V = 10.0f;

   QRectF NodeArranger::Arrange(NodeItem& node, const NodeArranger::Params& params)
   {
      return Arrange_Internal(node, params);
   }

   QRectF NodeArranger::Arrange_Internal(NodeItem& node, const NodeArranger::Params& params)
   {
      QRectF rect;
   
      NodeItemArray children;
      node.GetChildNodeItems(children);

      QRectF parentRect = node.boundingRect();

      float height = 0.0f;
      float width = 0.0f;
      float offsetX = 0.0f;
      float offsetY = parentRect.height() + params.mPaddingV;

      // Prevent further processing if this is simply a leaf node.
      size_t numChildren = children.size();
      if (numChildren <= 0)
      {
         return node.boundingRect();
      }

      size_t i = 0;
      std::for_each(children.begin(), children.end(),
         [&](NodeItem* child)
         {
            // Get the rectangle of the node itself.
            QRectF childOnlyRect = child->boundingRect();

            // Calculate the rectangle for the entire sub tree from the current node.
            QRectF rect = Arrange_Internal(*child, params);
            width += rect.width();

            // Determine if the node needs to be moved to the right
            // so that its subtree does not overlap the subtree of
            // the sibling node to the left.
            float widthOffset = (rect.width() - childOnlyRect.width()) * 0.5f;

            // Move the child to the right within the parent node space.
            child->setPos(offsetX + widthOffset, offsetY);

            // Track the maximum height for this tree's tier.
            float tmpH = rect.height();
            if (tmpH > height)
            {
               height = tmpH;
            }

            // Set the offsets if this is not the last child node.
            if (i + 1 < numChildren)
            {
               offsetX += params.mPaddingH + rect.width();
               width += params.mPaddingH;
            }

            ++i;
         }
      );

      height += params.mPaddingV;
      height += parentRect.height();


      // Center the collection of children under the parent.
      offsetX = width * -0.5f + parentRect.width() * 0.5f;

      std::for_each(children.begin(), children.end(),
         [&](NodeItem* child)
         {
            child->setPos(child->pos() + QPointF(offsetX, 0.0f));
         }
      );

      // Set the final rectangle for the whole node subtree.
      rect.setX(offsetX);
      rect.setY(0.0f);
      rect.setWidth(width);
      rect.setHeight(height);

      return rect;
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeGraphScene::NodeGraphScene()
      : mFloatNode(nullptr)
      , mAttachMode(false)
   {
      mConnectorManager = new NodeConnectorManager;
      mFloatingConnectorManager = new NodeConnectorManager;

      // Create a float node for pointing floating connectors to
      // when drawing connections to other nodes.
      dtCore::RefPtr<osg::Group> node = new osg::Group();
      node->setName("CONNECTOR_FLOAT_NODE");
      mFloatNode = new NodeItem(*new OsgNodeWrapper(*node));
      mFloatNode->setVisible(false);

      setBackgroundBrush(Qt::gray);

      CreateConnections();
   }

   NodeGraphScene::~NodeGraphScene()
   {
      delete mFloatNode;
      mFloatNode = nullptr;
   }

   void NodeGraphScene::CreateConnections()
   {
      connect(this, SIGNAL(selectionChanged()),
         this, SLOT(OnSelectionChanged()));
   }

   NodeItem* NodeGraphScene::AddNode(BaseNodeWrapper& node, bool addChildren)
   {
      NodeItem* nodeItem = nullptr;

      // Add the item only if it is not already referenced.
      if (FindNodeItem(node) == nullptr)
      {
         nodeItem = new NodeItem(node);
         addItem(nodeItem);
         nodeItem->CreateChildNodeItems(addChildren);

         mIdItemMap[node.GetId().ToString()] = nodeItem;

         mConnectorManager->CreateConnectorsToChildren(*nodeItem, addChildren);

         emit SignalNodeItemAdded(*nodeItem);
      }

      return nodeItem;
   }

   unsigned int NodeGraphScene::AddNodes(const BaseNodeWrapperArray& nodes, bool addChildren, NodeItemArray* outNodeItems)
   {
      unsigned int count = 0;

      std::for_each(nodes.begin(), nodes.end(),
         [&](BaseNodeWrapper* node)
         {
            NodeItem* nodeItem = AddNode(*node, addChildren);

            if (nodeItem != nullptr)
            {
               ++count;

               if (outNodeItems != nullptr)
               {
                  outNodeItems->push_back(nodeItem);
               }
            }
         }
      );

      return count;
   }

   bool NodeGraphScene::RemoveNode(BaseNodeWrapper& node)
   {
      bool success = false;

      NodeItem* nodeItem = FindNodeItem(node);

      if (nodeItem != nullptr)
      {
         DetachNodeItem(*nodeItem);

         IdNodeItemMap::iterator foundIter = mIdItemMap.find(node.GetId().ToString());
         if (foundIter != mIdItemMap.end())
         {
            mIdItemMap.erase(foundIter);
         }

         emit SignalNodeItemRemoved(*nodeItem);

         removeItem(nodeItem);

         success = true;
      }

      return success;
   }

   bool NodeGraphScene::RemoveNodeItem(NodeItem& nodeItem)
   {
      bool success = false;

      // Remove any internal id-to-nodeitem references.
      IdNodeItemMap::iterator iter = mIdItemMap.begin();
      for (; iter != mIdItemMap.end();)
      {
         if (iter->second == &nodeItem)
         {
            mIdItemMap.erase(iter++);
            success = true;
         }
         else
         {
            ++iter;
         }
      }

      // Remove the node graphics item if it was found.
      if (success)
      {
         DetachNodeItem(nodeItem);

         emit SignalNodeItemRemoved(nodeItem);

         removeItem(&nodeItem);
      }

      return success;
   }

   void NodeGraphScene::SetNodeProviderFunc(NodeProviderFunc nodeProviderFunc)
   {
      mNodeProviderFunc = nodeProviderFunc;
   }

   NodeItem* NodeGraphScene::FindNodeItem(BaseNodeWrapper& node) const
   {
      NodeItem* nodeItem = nullptr;

      IdNodeItemMap::const_iterator foundIter = mIdItemMap.find(node.GetId().ToString());
      if (foundIter != mIdItemMap.end())
      {
         nodeItem = foundIter->second;
      }

      return nodeItem;
   }

   unsigned int NodeGraphScene::SetNodesSelected(const BaseNodeWrapperArray& nodes, bool selected)
   {
      unsigned int count = 0;

      // Avoid sending out a multitude of signals during the loop.
      blockSignals(true);

      std::for_each(nodes.begin(), nodes.end(),
         [&](BaseNodeWrapper* node)
         {
            NodeItem* nodeItem = FindNodeItem(*node);

            if (nodeItem != nullptr && nodeItem->isSelected() != selected)
            {
               nodeItem->setSelected(selected);

               ++count;
            }
         }
      );
      
      // Re-enable signal functionality.
      blockSignals(false);

      // TODO: Determine if the signal should be sent out for the selection change.
      /*if (count > 0)
      {
         OnSelectionChanged();
      }*/

      return count;
   }

   unsigned int NodeGraphScene::SetSelectedNodes(const BaseNodeWrapperArray& nodes)
   {
      blockSignals(true);
      clearSelection();
      blockSignals(false);

      return SetNodesSelected(nodes, true);
   }

   unsigned int NodeGraphScene::GetSelectedNodes(BaseNodeWrapperArray& outNodes)
   {
      unsigned int count = 0;

      NodeItemArray nodeItems;
      if (GetSelectedNodeItems(nodeItems) > 0)
      {
         outNodes.reserve(nodeItems.size());

         std::for_each(nodeItems.begin(), nodeItems.end(),
            [&](NodeItem* node)
            {
               outNodes.push_back(&node->GetNodeWrapper());

               ++count;
            }
         );
      }

      return count;
   }

   unsigned int NodeGraphScene::GetSelectedNodeItems(NodeItemArray& outNodeItems)
   {
      unsigned int count = 0;

      QList<QGraphicsItem*> items = selectedItems();
      if ( ! items.empty())
      {
         outNodeItems.reserve(items.size());

         std::for_each(items.begin(), items.end(),
            [&](QGraphicsItem* item)
            {
               NodeItem* nodeItem = NodeItem::ConvertToNodeItem(*item);

               if (nodeItem != nullptr)
               {
                  outNodeItems.push_back(nodeItem);

                  ++count;
               }
            }
         );
      }

      return count;
   }

   void NodeGraphScene::GetNodesFromItems(const NodeItemArray& nodeItems, BaseNodeWrapperArray& outNodes)
   {
      outNodes.reserve(nodeItems.size());

      std::for_each(nodeItems.begin(), nodeItems.end(),
         [&](NodeItem* nodeItem)
         {
            outNodes.push_back(&nodeItem->GetNodeWrapper());
         }
      );
   }

   void NodeGraphScene::UpdateScene(NodeProviderFunc nodeProviderFunc)
   {
      if (! nodeProviderFunc.valid())
      {
         LOG_ERROR("No NodeProviderFunc was specified. Please assign a functor via SetNodeProviderFunc that will return node data to be displayed in the node graph scene.");
      }
      else // Node provider is valid...
      {
         ClearNodeItems();

         float offsetY = 0.0f;
         int index = 0;

         BaseNodeWrapperArray nodes;
         nodeProviderFunc(nodes);

         std::for_each(nodes.begin(), nodes.end(),
            [&](BaseNodeWrapper* node)
            {
               NodeItem* nodeItem = AddNode(*node, true);

               dtCore::RefPtr<NodeArranger> arranger = new NodeArranger;
               NodeArranger::Params params;
               params.mPaddingH = 40.0f;
               params.mPaddingV = 40.0f;
               QRectF totalTreeBounds = arranger->Arrange(*nodeItem, params);

               if (index > 0)
               {
                  nodeItem->setPos(nodeItem->pos() + QPointF(0.0f, offsetY));
               }

               offsetY += totalTreeBounds.height() + params.mPaddingV;
               ++index;
            }
         );
      }
   }

   void NodeGraphScene::UpdateScene()
   {
      UpdateScene(mNodeProviderFunc);
   }

   void NodeGraphScene::DetachNodeItem(NodeItem& nodeItem)
   {
      QPointF scenePos = nodeItem.scenePos();

      nodeItem.setParentItem(nullptr);
      nodeItem.setPos(scenePos);

      mConnectorManager->RemoveConnectorToParent(nodeItem);

      emit SignalNodeDetached(nodeItem.GetNodeWrapper());
   }

   void NodeGraphScene::DetachNodeItems(const NodeItemArray& nodeItems)
   {
      if (! nodeItems.empty())
      {
         std::for_each(nodeItems.begin(), nodeItems.end(),
            [&](NodeItem* nodeItem)
            {
               DetachNodeItem(*nodeItem);
            }
         );

         BaseNodeWrapperArray nodes;
         GetNodesFromItems(nodeItems, nodes);

         emit SignalNodesDetached(nodes);
      }
   }

   void NodeGraphScene::AttachNodeItem(NodeItem& nodeItem, NodeItem& parentNodeItem)
   {
      if (&nodeItem != &parentNodeItem)
      {
         QPointF scenePos = nodeItem.scenePos();

         nodeItem.setParentItem(&parentNodeItem);
         nodeItem.setPos(parentNodeItem.mapFromScene(scenePos));

         mConnectorManager->CreateConnector(parentNodeItem, nodeItem);

         emit SignalNodeAttached(nodeItem.GetNodeWrapper(), parentNodeItem.GetNodeWrapper());
      }
   }

   void NodeGraphScene::AttachNodeItems(const NodeItemArray& nodeItems, NodeItem& parentNodeItem)
   {
      if ( ! nodeItems.empty())
      {
         std::for_each(nodeItems.begin(), nodeItems.end(),
            [&](NodeItem* nodeItem)
            {
               AttachNodeItem(*nodeItem, parentNodeItem);
            }
         );

         BaseNodeWrapperArray nodes;
         GetNodesFromItems(nodeItems, nodes);

         emit SignalNodesAttached(nodes, parentNodeItem.GetNodeWrapper());
      }
   }

   void NodeGraphScene::DetachSelectedNodes()
   {
      NodeItemArray nodeItems;
      if (GetSelectedNodeItems(nodeItems) > 0)
      {
         DetachNodeItems(nodeItems);
      }
   }

   void NodeGraphScene::AttachSelectedNodes(NodeItem& parentNodeItem)
   {
      NodeItemArray nodeItems;
      if (GetSelectedNodeItems(nodeItems) > 0)
      {
         AttachNodeItems(nodeItems, parentNodeItem);
      }
   }

   void NodeGraphScene::SetFloatingConnectorsEndPosition(const QPointF& pos)
   {
      // Connector ends are based on node center points.
      // Find the offset to get the top left corner.
      QRectF rect = mFloatNode->boundingRect();
      QPointF offset(rect.width(), rect.height());
      offset *= -0.5f;

      mFloatNode->setPos(pos + offset);
   }

   void NodeGraphScene::CreateFloatingConnectorsToNodeItems(const NodeItemArray& nodeItems)
   {
      mFloatingConnectorManager->Clear();
      NodeConnectorArray connectors = mFloatingConnectorManager->CreateConnectors(*mFloatNode, nodeItems, false);

      std::for_each(connectors.begin(), connectors.end(),
         [&](NodeConnector* connector)
         {
            // Have the connector calculate node positions in absolute scene coordinates.
            connector->SetAbsoluteMode(true);

            addItem(connector);
         }
      );
   }

   void NodeGraphScene::UpdateFloatingConnectors()
   {
      mFloatingConnectorManager->UpdateConnectors();
   }

   void NodeGraphScene::ClearFloatingConnectors()
   {
      mFloatingConnectorManager->Clear();
   }

   bool NodeGraphScene::IsAttachEnabled() const
   {
      return mAttachMode;
   }

   void NodeGraphScene::OnSelectionChanged()
   {
      BaseNodeWrapperArray nodes;
      GetSelectedNodes(nodes);

      emit SignalNodesSelected(nodes);
   }

   void NodeGraphScene::OnDetachAction()
   {
      DetachSelectedNodes();
   }

   void NodeGraphScene::OnAttachAction(bool attachEnabled)
   {
      if (attachEnabled)
      {
         // TODO:
      }
      else
      {
         ClearFloatingConnectors();
      }

      mAttachMode = attachEnabled;
   }

   void NodeGraphScene::ClearNodeItems()
   {
      clear();

      mIdItemMap.clear();

      emit SignalCleared();
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeGraphView::NodeGraphView()
      : mZoom(1.0f)
      , mZoomMin(0.1f)
      , mZoomMax(2.0f)
   {
      mNodeGraphScene = new NodeGraphScene;
      setScene(mNodeGraphScene);

      setDragMode(QGraphicsView::ScrollHandDrag);
   }

   NodeGraphView::~NodeGraphView()
   {}

   NodeGraphScene* NodeGraphView::GetNodeGraphScene()
   {
      return mNodeGraphScene;
   }

   const NodeGraphScene* NodeGraphView::GetNodeGraphScene() const
   {
      return mNodeGraphScene;
   }

   NodeItem* NodeGraphView::GetNodeItemAt(const QPoint& viewPos) const
   {
      QGraphicsItem* item = itemAt(viewPos);
      return item == nullptr ? nullptr : NodeItem::ConvertToNodeItem(*item);
   }

   void NodeGraphView::SetZoom(float zoom)
   {
      if (mZoom != zoom)
      {
         dtUtil::Clamp(zoom, mZoomMin, mZoomMax);
         mZoom = zoom;

         QTransform oldXform = transform();

         QTransform xform;
         xform.translate(oldXform.dx(), oldXform.dy());
         xform.scale(zoom, zoom);
         setTransform(xform);

         emit SignalZoomChanged(zoom);
      }
   }

   float NodeGraphView::GetZoom() const
   {
      return mZoom;
   }

   void NodeGraphView::SetZoomMin(float zoom)
   {
      mZoomMin = zoom;
   }

   float NodeGraphView::GetZoomMin() const
   {
      return mZoomMin;
   }

   void NodeGraphView::SetZoomMax(float zoom)
   {
      mZoomMax = zoom;
   }

   float NodeGraphView::GetZoomMax() const
   {
      return mZoomMax;
   }

   void NodeGraphView::mousePressEvent(QMouseEvent* mouseEvent)
   {
      BaseClass::mousePressEvent(mouseEvent);

      //if ( ! mouseEvent->isAccepted())
      {
         NodeItem* node = GetNodeItemAt(mouseEvent->pos());

         if (node != nullptr)
         {
            SignalNodeItemOnMouseClick(*node);

            mouseEvent->accept();

            // Start dragging a connection line if nodes were selected.
            bool attachMode = mNodeGraphScene->IsAttachEnabled();
            if (attachMode)
            {
               NodeItemArray nodeItems;
               mNodeGraphScene->GetSelectedNodeItems(nodeItems);

               if ( ! nodeItems.empty())
               {
                  mNodeGraphScene->CreateFloatingConnectorsToNodeItems(nodeItems);

                  QPointF scenePos = mapToScene(mouseEvent->pos());
                  mNodeGraphScene->SetFloatingConnectorsEndPosition(scenePos);
                  mNodeGraphScene->UpdateFloatingConnectors();
               }
            }

            node->SetMovable( ! attachMode);
         }
      }
   }

   void NodeGraphView::mouseReleaseEvent(QMouseEvent* mouseEvent)
   {
      BaseClass::mouseReleaseEvent(mouseEvent);

      //if ( ! mouseEvent->isAccepted())
      {
         NodeItem* node = GetNodeItemAt(mouseEvent->pos());

         /*if (node != nullptr)
         {
            mouseEvent->accept();
         }*/

         SignalNodeItemOnMouseRelease(node);

         // End dragging a connection line if nodes were selected.
         if (mNodeGraphScene->IsAttachEnabled())
         {
            NodeItemArray nodeItems;
            mNodeGraphScene->GetSelectedNodeItems(nodeItems);

            // Remove the old connectors.
            mNodeGraphScene->DetachNodeItems(nodeItems);

            // Remove the floating connectors.
            mNodeGraphScene->ClearFloatingConnectors();

            // Complete the attempted attach action.
            if (node != nullptr && ! nodeItems.empty())
            {
               mNodeGraphScene->AttachNodeItems(nodeItems, *node);
            }
         }
      }
   }

   void NodeGraphView::mouseMoveEvent(QMouseEvent* mouseEvent)
   {
      BaseClass::mouseMoveEvent(mouseEvent);

      //if ( ! mouseEvent->isAccepted())
      {
         // Update the end position of floating connectors if attach mode is enabled.
         if (mNodeGraphScene->IsAttachEnabled())
         {
            QPointF scenePos = mapToScene(mouseEvent->pos());
            mNodeGraphScene->SetFloatingConnectorsEndPosition(scenePos);
            mNodeGraphScene->UpdateFloatingConnectors();
         }
      }
   }

   void NodeGraphView::wheelEvent(QWheelEvent* wheelEvent)
   {
      BaseClass::wheelEvent(wheelEvent);

      //if ( ! wheelEvent->isAccepted())
      {
         SetZoom(mZoom + (wheelEvent->delta() > 0 ? 0.1f : -0.1f));
         
         wheelEvent->accept();
      }
   }

   void NodeGraphView::OnZoomAction(const QString zoomValue)
   {
      bool valid = true;

      std::string str(zoomValue.toLatin1());
      dtUtil::FindAndReplace(str, "%", "");

      float zoom = QString(str.c_str()).toFloat(&valid) / 100.0f;

      if (valid)
      {
         SetZoom(zoom);
      }
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeGraphViewerPanel::NodeGraphViewerPanel()
      : mUI(new Ui::NodeGraphViewer)
      , mGraphView(nullptr)
   {
      mUI->setupUi(this);

      mGraphView = new NodeGraphView;

      if (mUI->mView->layout() == nullptr)
      {
         QVBoxLayout* vlayout = new QVBoxLayout;
         mUI->mView->setLayout(vlayout);
      }

      mUI->mView->layout()->addWidget(mGraphView);

      mGraphView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

      CreateConnections();
   }

   NodeGraphViewerPanel::~NodeGraphViewerPanel()
   {
      delete mUI;
      mUI = nullptr;
   }

   NodeGraphView& NodeGraphViewerPanel::GetNodeGraphView()
   {
      return *mGraphView;
   }

   const NodeGraphView& NodeGraphViewerPanel::GetNodeGraphView() const
   {
      return *mGraphView;
   }

   void NodeGraphViewerPanel::UpdateUI()
   {
      // TODO:
   }

   void NodeGraphViewerPanel::CreateConnections()
   {
      // ZOOM
      // --- Control zoom factor by combobox selection action.
      connect(mUI->mZoom, SIGNAL(editTextChanged(const QString)),
         mGraphView, SLOT(OnZoomAction(const QString)));
      connect(mUI->mZoom, SIGNAL(currentIndexChanged(const QString)),
         mGraphView, SLOT(OnZoomAction(const QString)));
      // --- Update the zoom display value as the zoom factor changes.
      connect(mGraphView, SIGNAL(SignalZoomChanged(float)),
         this, SLOT(OnZoomChanged(float)));

      // DETACH
      connect(mUI->mBtnModeDetach, SIGNAL(clicked()),
         mGraphView->GetNodeGraphScene(), SLOT(OnDetachAction()));

      // ATTACH
      connect(mUI->mBtnModeAttach, SIGNAL(clicked(bool)),
         mGraphView->GetNodeGraphScene(), SLOT(OnAttachAction(bool)));
   }

   void NodeGraphViewerPanel::OnZoomChanged(float value)
   {
      // Disable signals temporarily to prevent OnZoomValueChanged from triggering,
      // which could cause unwanted processing.
      blockSignals(true);

      mUI->mZoom->setCurrentText(QString::number(value * 100.0f) + "%");

      blockSignals(false);
   }

} // END - namespace dtQt
