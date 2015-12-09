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
#include <dtQt/nodegraphview.h>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainterPath>
#include <osg/Group>
#include <dtQt/constants.h>
#include <dtUtil/mathdefines.h>
#include <sstream>



namespace dtQt
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   NodeItem::NodeItem(osg::Node& node)
      : mCollapsed(false)
      , mNode(&node)
   {
      setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

      setZValue(1.0f);

      std::ostringstream oss;
      typedef osg::Node::DescriptionList DescList;
      DescList descList = node.getDescriptions();

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

      setToolTip(oss.str().c_str());
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

      osg::Group* group = mNode->asGroup();
   
      if (group != nullptr)
      {
         unsigned int numChildren = group->getNumChildren();
         nodeItemArray.reserve(numChildren);

         NodeItem* curNodeItem = nullptr;
         osg::Node* curNode = nullptr;
         for (unsigned int i = 0; i < numChildren; ++i)
         {
            curNode = group->getChild(i);

            curNodeItem = new NodeItem(*curNode);
            curNodeItem->setParentItem(this);
            nodeItemArray.push_back(curNodeItem);
         }

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
      painter->drawText(textRect, mNode->getName().c_str(), op);

      std::string nodeClass(mNode->className());
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
      pen.setWidth(1.5f);

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

      size_t numChildren = children.size();
      if (numChildren <= 0)
      {
         return node.boundingRect();
      }

      size_t i = 0;
      std::for_each(children.begin(), children.end(),
         [&](NodeItem* child)
         {
            QRectF rect = Arrange_Internal(*child, params);
            width += rect.width();

            child->setPos(offsetX, offsetY);

            float tmpH = rect.height();
            if (tmpH > height)
            {
               height = tmpH;
            }

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
      if (numChildren > 1)
      {
         offsetX = width * -0.5f + parentRect.width() * 0.5f;

         std::for_each(children.begin(), children.end(),
            [&](NodeItem* child)
            {
               child->setPos(child->pos() + QPointF(offsetX, 0.0f));
            }
         );
      }

      // Set the final rect.
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

   void NodeGraphScene::SetSceneNode(osg::Node* node)
   {
      if (mScene != node)
      {
         mScene = node;

         UpdateScene();
      }
   }

   void NodeGraphScene::UpdateScene()
   {
      clear();

      if (mScene.valid())
      {
         NodeItem* nodeItem = new NodeItem(*mScene);
         addItem(nodeItem);
         nodeItem->CreateChildNodeItems(true);

         dtCore::RefPtr<NodeConnectorManager> connMgr = new NodeConnectorManager;
         connMgr->CreateConnectors(*nodeItem, true);

         dtCore::RefPtr<NodeArranger> arranger = new NodeArranger;
         NodeArranger::Params params;
         params.mPaddingH = 40.0f;
         params.mPaddingV = 40.0f;
         arranger->Arrange(*nodeItem, params);
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
         vlayout->addWidget(mGraphView);
      }
      else
      {
         mUI->mView->layout()->addWidget(mGraphView);
      }
      mGraphView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   }

   NodeGraphViewerPanel::~NodeGraphViewerPanel()
   {}

   NodeGraphView& NodeGraphViewerPanel::GetGraphView()
   {
      return *mGraphView;
   }

   const NodeGraphView& NodeGraphViewerPanel::GetGraphView() const
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
