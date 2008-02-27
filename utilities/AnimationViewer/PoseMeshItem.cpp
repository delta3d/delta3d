#include "PoseMeshItem.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneEvent>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QCursor>

#include <QtGui/QMouseEvent>
#include <QtGui/QHoverEvent>

#include <dtAnim/PoseMesh.h>
#include <dtAnim/chardrawable.h>
#include <dtCore/refptr.h>

#include <assert.h>

//temp
#include <sstream>
#include <iostream>
#include <dtAnim/PoseMeshUtility.h>

const float VERT_SCALE     = 100.0f;
const int VERT_RADIUS      = 6;
const int VERT_RADIUS_DIV2 = VERT_RADIUS / 2;

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshItem::PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                           dtAnim::Cal3DModelWrapper *model,
                           QGraphicsItem *parent, 
                           QGraphicsScene *scene)
  : mPoseMesh(&poseMesh)
  , mModel(model)
  , mBoundingRect()
  , mLastBlendPos(FLT_MAX, FLT_MAX)
  , mLastTriangleID(INT_MAX)
{
   assert(mModel);

   // We want to manually control when the item is movable
   setFlag(ItemIsMovable, false);

   // Make sure this gets drawn on top of the background
   setZValue(1);

   setAcceptsHoverEvents(true);
   setToolTip("test");  

   mHovered = false;  
   
   //QSize size(40, 20);
   //QImage image(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
   //image.fill(qRgba(0, 0, 0, 0));

   //QFont font;
   //font.setStyleStrategy(QFont::ForceOutline);

   //QPainter painter;
   //painter.begin(&image);
   //painter.setRenderHint(QPainter::Antialiasing);
   //painter.setBrush(Qt::white);
   //painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1), 25, 25);

   ////painter.setFont(font);
   ////painter.setBrush(Qt::black);
   ////painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
   //painter.end();

   //mPixmap = new QPixmap;
   //*mPixmap = QPixmap::fromImage(image);

   const dtAnim::PoseMesh::VertexVector &verts = mPoseMesh->GetVertices();

   // Determine the bounding box for this item
   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      osg::Vec3 vertPosition = verts[vertIndex]->mData * VERT_SCALE;      

      // Set the horizontal bounds
      if (vertPosition.x() < mBoundingRect.left())
      {
         mBoundingRect.setLeft(vertPosition.x());
      }
      else if (vertPosition.x() > mBoundingRect.right())
      {
         mBoundingRect.setRight(vertPosition.x());
      }

      // Set the vertical bounds
      if (-vertPosition.y() < mBoundingRect.top())
      {
         mBoundingRect.setTop(-vertPosition.y());
      }
      else if (-vertPosition.y() > mBoundingRect.bottom())
      {
         mBoundingRect.setBottom(-vertPosition.y());
      }      
   }   

   // Adjust the box to account for the vert radius
   mBoundingRect.adjust(-VERT_RADIUS_DIV2,
                        -VERT_RADIUS_DIV2,
                        VERT_RADIUS_DIV2,
                        VERT_RADIUS_DIV2);

   // Get a unique set of edges and the 
   // triangle they belong to
   ExtractEdgesFromMesh(*mPoseMesh);
}

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshItem::~PoseMeshItem()
{

}

/////////////////////////////////////////////////////////////////////////////////////////
const std::string& PoseMeshItem::GetPoseMeshName()
{
   return mPoseMesh->GetName(); 
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetEnabled(bool isEnabled)
{
   setEnabled(isEnabled);
   setVisible(isEnabled);

   // Remove any of this item's pose blends from the model
   dtCore::RefPtr<dtAnim::PoseMeshUtility> util = new dtAnim::PoseMeshUtility;
   util->ClearPoses(mPoseMesh, mModel, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::sceneEvent(QEvent *event)
{
   switch(event->type())
   {
      case QEvent::GraphicsSceneHoverEnter:
      {
         mHovered = true;    
         update(mBoundingRect);
         return true;
      }
      case QEvent::GraphicsSceneHoverLeave:
      {
         mHovered = false;
         update(mBoundingRect);
         return true;
      }  
   }
   
   //std::ostringstream oss;
   //oss << "event: " << event->type();
   //std::cout << oss.str() << std::endl;

   return QGraphicsItem::sceneEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{   
   if (event->button() == Qt::LeftButton)
   {     
      // Convert the position back to it's unscaled form
      mLastMousePos = event->lastPos(); 
            
      //std::ostringstream oss;
      //oss << "pos: (" << mLastMousePos.x() << ", " << mLastMousePos.y() << ")";
      //std::cout << oss.str() << std::endl;

      BlendPosesFromItemCoordinates(mLastMousePos.x(), mLastMousePos.y());     
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
   // Update our blend to the latest and greatest position
   mLastMousePos = event->pos();
   BlendPosesFromItemCoordinates(mLastMousePos.x(), mLastMousePos.y());

   QGraphicsItem::mouseMoveEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::BlendPosesFromItemCoordinates(float xCoord, float yCoord)
{
   // Scale back into pose space and flip the y coord
   dtAnim::PoseMesh::TargetTriangle targetTri;
   mPoseMesh->GetTargetTriangleData(xCoord / VERT_SCALE, -yCoord / VERT_SCALE, targetTri);

   // Only update the blend and position if we're in the mesh
   if (targetTri.mIsInside)
   {      
      dtCore::RefPtr<dtAnim::PoseMeshUtility> util = new dtAnim::PoseMeshUtility;
      util->BlendPoses(mPoseMesh, mModel, targetTri);

      //std::ostringstream oss;
      //oss << "x = " << xCoord << "  " << "y = " << yCoord 
      //    << "  "   << "tri = " << targetTri.mTriangleID;

      //std::cout << oss.str() << std::endl;

      // Store the last valie visual location of the blend
      mLastBlendPos.setX(xCoord);
      mLastBlendPos.setY(yCoord);
      mLastTriangleID = targetTri.mTriangleID;
   }   

   // Make sure to redraw the changed portion
   update(boundingRect());
}

/////////////////////////////////////////////////////////////////////////////////////////
QRectF PoseMeshItem::boundingRect() const
{
   return mBoundingRect;
   //qreal adjust = 5;
   //return mBoundingRect.adjusted(adjust, adjust, adjust, adjust);
}

/////////////////////////////////////////////////////////////////////////////////////////
QPainterPath PoseMeshItem::shape() const
{
   QPainterPath path;
   //path.addEllipse(-5, -5, 10, 10);
   path.addRect(mBoundingRect);
   return path;
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   const dtAnim::PoseMesh::VertexVector &verts = mPoseMesh->GetVertices();

   QPen trianglePenDefault;   
   trianglePenDefault.setColor(Qt::black);

   QPen trianglePenSelected;
   trianglePenSelected.setColor(Qt::green);

   for (size_t edgeIndex = 0; edgeIndex < mEdgeInfoList.size(); ++edgeIndex)
   {
      if (mEdgeInfoList[edgeIndex].triangleIDs[0] == mLastTriangleID ||
          mEdgeInfoList[edgeIndex].triangleIDs[1] == mLastTriangleID)
      {
         painter->setPen(trianglePenSelected);
      }
      else
      {
         painter->setPen(trianglePenDefault);
      }

      painter->drawLine(mEdgeInfoList[edgeIndex].first, mEdgeInfoList[edgeIndex].second);
   }

   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      osg::Vec3 vertPosition = verts[vertIndex]->mData * VERT_SCALE;

      QRadialGradient gradient(-3, -3, 10);
      if (option->state & QStyle::State_Sunken) 
      {
         gradient.setCenter(3, 3);
         gradient.setFocalPoint(3, 3);
         gradient.setColorAt(1, QColor(Qt::yellow).light(120));
         gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
      }
      else
      {        
         if (mHovered)
         {
            gradient.setColorAt(0, Qt::red);
            gradient.setColorAt(1, Qt::darkRed);
         }
         else
         {
            gradient.setColorAt(0, Qt::yellow);
            gradient.setColorAt(1, Qt::darkYellow);
         }         
      }

      painter->translate(vertPosition.x(), -vertPosition.y());
      painter->setBrush(gradient);
      painter->setPen(QPen(Qt::black, 0));
      painter->drawEllipse(-VERT_RADIUS_DIV2, -VERT_RADIUS_DIV2, VERT_RADIUS, VERT_RADIUS);
      painter->translate(-vertPosition.x(), vertPosition.y());

      QPen boundPen;
      boundPen.setStyle(Qt::DotLine);
      boundPen.setColor(Qt::black);

      painter->setPen(boundPen);
      painter->setBrush(QBrush());
      painter->drawRect(mBoundingRect);
      
      // Translating by FLT_MAX is no bueno
      if (mLastBlendPos.x() != FLT_MAX)
      {
         QPen targetPen;
         targetPen.setStyle(Qt::DotLine);
         targetPen.setWidth(1);

         painter->translate(mLastBlendPos);
         painter->setPen(targetPen);
         painter->setBrush(QBrush(QColor(255, 255, 255), Qt::SolidPattern));
         painter->drawEllipse(-VERT_RADIUS_DIV2, -VERT_RADIUS_DIV2, VERT_RADIUS, VERT_RADIUS);
         painter->translate(-mLastBlendPos);
      }      
   }   
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::ExtractEdgesFromMesh(const dtAnim::PoseMesh &mesh)
{
   typedef std::pair<dtAnim::PoseMesh::MeshIndexPair, int> VertIndexTriPair;
   typedef std::multimap<dtAnim::PoseMesh::MeshIndexPair, int> EdgeTriMap;

   EdgeTriMap edgeMap;

   const dtAnim::PoseMesh::TriangleVector &triList = mesh.GetTriangles();
   const dtAnim::PoseMesh::VertexVector &vertList = mesh.GetVertices();  

   std::vector<dtAnim::PoseMesh::MeshIndexPair> edgeSet;

   // Go through each triangle and get a set of unique
   // edges and the triangles that contain them
   for (size_t triIndex = 0; triIndex < triList.size(); ++triIndex)
   {
      // Make 3 pairs of points to represent 3 edges of the tri
      dtAnim::PoseMesh::MeshIndexPair pair[3];
      mesh.GetIndexPairsForTriangle(triIndex, pair[0], pair[1], pair[2]);

      // Swapped pairs are considered equivalent, don't duplicate them
      dtAnim::PoseMesh::MeshIndexPair swapped[3];
      for (size_t pairIndex = 0; pairIndex < 3; ++pairIndex)
      {         
         swapped[pairIndex].first  = pair[pairIndex].second;
         swapped[pairIndex].second = pair[pairIndex].first;
      }

      for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
      {
         EdgeTriMap::iterator pairIter = edgeMap.find(pair[edgeIndex]);
         EdgeTriMap::iterator swapIter = edgeMap.find(swapped[edgeIndex]);

         // Only use one ordering for each edge
         if (pairIter != edgeMap.end())
         {
            edgeMap.insert(VertIndexTriPair(pair[edgeIndex], triIndex));
         }
         else if (swapIter != edgeMap.end())
         {
            edgeMap.insert(VertIndexTriPair(swapped[edgeIndex], triIndex));           
         }
         else
         {
            // Store the new edge that we haven't seen before
            edgeMap.insert(VertIndexTriPair(pair[edgeIndex], triIndex));

            // Store each unique edge when added for the first time
            // This is used to construct the final edge list vector
            edgeSet.push_back(pair[edgeIndex]);
         }
      }
   } // for each triangle

   // For each unique edge
   for (size_t setIndex = 0; setIndex < edgeSet.size(); ++setIndex)
   {
      dtAnim::PoseMesh::MeshIndexPair key = edgeSet[setIndex];
      EdgeTriMap::iterator rangeStart = edgeMap.find(key);
      EdgeTriMap::iterator rangeEnd   = edgeMap.upper_bound(key);
      
      assert(rangeStart != edgeMap.end());

      dtAnim::PoseMesh::Vertex *vert0 = vertList[key.first];
      dtAnim::PoseMesh::Vertex *vert1 = vertList[key.second];

      EdgeInfo newInfo;
      newInfo.first.rx()  = vert0->mData.x() * VERT_SCALE;
      newInfo.first.ry()  = vert0->mData.y() * -VERT_SCALE;  
      newInfo.second.rx() = vert1->mData.x() * VERT_SCALE;
      newInfo.second.ry() = vert1->mData.y() * -VERT_SCALE;  

      newInfo.triangleIDs[0] = rangeStart->second;

      if (++rangeStart != rangeEnd)
      {
         newInfo.triangleIDs[1] = rangeStart->second;
         assert(++rangeStart == rangeEnd);
      }      
      else
      {
         newInfo.triangleIDs[1] = -1;
      }

      mEdgeInfoList.push_back(newInfo);
   }
}