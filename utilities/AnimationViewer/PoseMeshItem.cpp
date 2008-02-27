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
{
   assert(mModel);

   setFlag(ItemIsMovable, false);
   setZValue(1);

   setAcceptsHoverEvents(true);
   setToolTip("test");  

   mHovered = false;

   mCursor = new QCursor(QPixmap(":/images/reticle.png"));
   setCursor(*mCursor);
   
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

   for (size_t edgeIndex = 0; edgeIndex < mEdgeList.size(); ++edgeIndex)
   {
      painter->drawLine(mEdgeList[edgeIndex].first, mEdgeList[edgeIndex].second);
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
   const dtAnim::PoseMesh::TriangleVector &triList = mesh.GetTriangles();
   const dtAnim::PoseMesh::VertexVector &vertList = mesh.GetVertices();

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
         // Make sure that each edge is unique before adding it
         if (mEdgeSet.find(pair[edgeIndex]) == mEdgeSet.end() &&
             mEdgeSet.find(swapped[edgeIndex]) == mEdgeSet.end())
         {
            dtAnim::PoseMesh::Vertex *vert0 = vertList[pair[edgeIndex].first];
            dtAnim::PoseMesh::Vertex *vert1 = vertList[pair[edgeIndex].second];

            QPointF point0(vert0->mData.x(), vert0->mData.y());
            point0.rx() *= VERT_SCALE;
            point0.ry() *= -VERT_SCALE;

            QPointF point1(vert1->mData.x(), vert1->mData.y());
            point1.rx() *= VERT_SCALE;
            point1.ry() *= -VERT_SCALE;        

            typedef std::pair<QPointF, QPointF> EdgePair;
            mEdgeList.push_back(EdgePair(point0, point1));
         }
      }
   }
}