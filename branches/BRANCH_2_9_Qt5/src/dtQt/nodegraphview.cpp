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
      : mNodeA(&nodeA)
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

   QRectF NodeConnector::boundingRect() const
   {
      QPointF pos = scenePos();
      QPointF ptA = mapToParent(mNodeA->GetCenter() - pos) + QPointF(0.0f, mNodeA->GetHeight() * 0.5f);
      QPointF ptB = mapToParent(mNodeB->GetCenter() - pos) - QPointF(0.0f, mNodeB->GetHeight() * 0.5f);

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

      painter.setBrush(Qt::red);

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

   NodeConnectorArray NodeConnectorManager::CreateConnectors(NodeItem& node, bool recurse)
   {
      NodeConnectorArray connectorArray;

      NodeItemArray children;
      node.GetChildNodeItems(children);
      connectorArray.reserve(children.size());

      NodeConnector* connector = nullptr;
      std::for_each(children.begin(), children.end(),
         [&](NodeItem* childNode)
         {
            connector = new NodeConnector(node, *childNode);
            connector->setParentItem(&node);
            connectorArray.push_back(connector);

            QObject::connect(childNode, SIGNAL(SignalMoved()),
               connector, SLOT(OnNodeMoved()));
         }
      );

      // Determine if creation of connectors should recurse through the node tree.
      if (recurse)
      {
         std::for_each(children.begin(), children.end(),
            [&](NodeItem* childNode)
            {
               CreateConnectors(*childNode, true);
            }
         );
      }

      return connectorArray;
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
   {
      QBrush brush;
      brush.setColor(Qt::gray);
   
      setBackgroundBrush(Qt::gray);

      // DEBUG:
      /*dtCore::RefPtr<osg::Group> childA = new osg::Group;
      childA->setName("A");
      dtCore::RefPtr<osg::Group> childB = new osg::Group;
      childB->setName("B");
      dtCore::RefPtr<osg::Group> node = new osg::Group;
      node->setName("Test Node");
      node->addChild(childA.get());
      node->addChild(childB.get());

      SetSceneNode(node.get());*/
   
      connect(this, SIGNAL(selectionChanged()),
         this, SLOT(OnSelectionChanged()));
   }

   NodeGraphScene::~NodeGraphScene()
   {}

   void NodeGraphScene::SetSceneNodes(const BaseNodeWrapperArray& nodes)
   {
      mSceneNodes = nodes;

      UpdateScene();
   }

   unsigned int NodeGraphScene::GetSelectedNodes(BaseNodeWrapperArray& outNodes)
   {
      unsigned int count = 0;

      QList<QGraphicsItem*> items = selectedItems();
      if ( ! items.empty())
      {
         outNodes.reserve(items.size());

         std::for_each(items.begin(), items.end(),
            [&](QGraphicsItem* item)
            {
               NodeItem* nodeItem = NodeItem::ConvertToNodeItem(*item);

               if (nodeItem != nullptr)
               {
                  outNodes.push_back(&nodeItem->GetNodeWrapper());

                  ++count;
               }
            }
         );
      }

      return count;
   }

   void NodeGraphScene::UpdateScene()
   {
      clear();

      if (! mSceneNodes.empty())
      {
         float offsetY = 0.0f;
         int index = 0;

         std::for_each(mSceneNodes.begin(), mSceneNodes.end(),
            [&](BaseNodeWrapper* node)
            {
               NodeItem* nodeItem = new NodeItem(*node);
               addItem(nodeItem);
               nodeItem->CreateChildNodeItems(true);

               dtCore::RefPtr<NodeConnectorManager> connMgr = new NodeConnectorManager;
               connMgr->CreateConnectors(*nodeItem, true);

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

   void NodeGraphScene::OnSelectionChanged()
   {
      // TODO:
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

   void NodeGraphView::SetZoom(float zoom)
   {
      dtUtil::Clamp(zoom, mZoomMin, mZoomMax);
      mZoom = zoom;

      QTransform oldXform = transform();

      QTransform xform;
      xform.translate(oldXform.dx(), oldXform.dy());
      xform.scale(zoom, zoom);
      setTransform(xform);
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

      if ( ! mouseEvent->isAccepted())
      {
         // TODO:
      }
   }

   void NodeGraphView::mouseReleaseEvent(QMouseEvent* mouseEvent)
   {
      BaseClass::mouseReleaseEvent(mouseEvent);

      if ( ! mouseEvent->isAccepted())
      {
         // TODO:
      }
   }

   void NodeGraphView::mouseMoveEvent(QMouseEvent* mouseEvent)
   {
      BaseClass::mouseMoveEvent(mouseEvent);

      if ( ! mouseEvent->isAccepted())
      {
         // TODO:
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
      // TODO:
   }

} // END - namespace dtQt
