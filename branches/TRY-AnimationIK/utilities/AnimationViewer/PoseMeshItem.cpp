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

#include <QtGui/QMouseEvent>
#include <QtGui/QHoverEvent>

#include <dtAnim/PoseMesh.h>

const float VERT_SCALE     = 100.0f;
const int VERT_RADIUS      = 6;
const int VERT_RADIUS_DIV2 = VERT_RADIUS / 2;


PoseMeshItem::PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                           QGraphicsItem *parent, 
                           QGraphicsScene *scene)
  : mPoseMesh(&poseMesh)
  , mBoundingRect()
{
   setFlag(ItemIsMovable);
   setZValue(1);

   setAcceptsHoverEvents(true);

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
      if (vertPosition.y() < mBoundingRect.top())
      {
         mBoundingRect.setTop(vertPosition.y());
      }
      else if (vertPosition.y() > mBoundingRect.bottom())
      {
         mBoundingRect.setBottom(vertPosition.y());
      }      
   }   

   // Adjust the box to account for the vert radius
   mBoundingRect.adjust(-VERT_RADIUS_DIV2,
                        -VERT_RADIUS_DIV2,
                        VERT_RADIUS_DIV2,
                        VERT_RADIUS_DIV2);
}


PoseMeshItem::~PoseMeshItem()
{

}

const std::string& PoseMeshItem::GetPoseMeshName()
{
   return mPoseMesh->GetName(); 
}

bool PoseMeshItem::sceneEvent(QEvent *event)
{
   if (event->type() == QEvent::GraphicsSceneHoverEnter)
   {
      mHovered = true;    
      update(mBoundingRect);
      return true;
   }
   else if (event->type() == QEvent::GraphicsSceneHoverLeave)
   {
      mHovered = false;
      update(mBoundingRect);
      return true;
   }

   return QGraphicsItem::sceneEvent(event);
}

void PoseMeshItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{  
   QMimeData *data = new QMimeData;
   data->setColorData(Qt::green);
   
   QDrag *drag = new QDrag(event->widget());
   drag->setMimeData(data);
   //drag->setPixmap(*mPixmap);   
   drag->start(Qt::MoveAction);
   show();  
}

void PoseMeshItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

void PoseMeshItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
   mLastMousePos = event->pos();
   QGraphicsItem::mouseMoveEvent(event);
}

QRectF PoseMeshItem::boundingRect() const
{
   return mBoundingRect;
   //qreal adjust = 5;
   //return mBoundingRect.adjusted(adjust, adjust, adjust, adjust);
}

QPainterPath PoseMeshItem::shape() const
{
   QPainterPath path;
   //path.addEllipse(-5, -5, 10, 10);
   path.addRect(mBoundingRect);
   return path;
}

void PoseMeshItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   const dtAnim::PoseMesh::VertexVector &verts = mPoseMesh->GetVertices();

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
         //if (mBoundingRect.contains(mapToScene(mLastMousePos)))
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

      painter->translate(vertPosition.x(), vertPosition.y());
      painter->setBrush(gradient);
      painter->setPen(QPen(Qt::black, 0));
      painter->drawEllipse(-VERT_RADIUS_DIV2, -VERT_RADIUS_DIV2, VERT_RADIUS, VERT_RADIUS);
      painter->translate(-vertPosition.x(), -vertPosition.y());

      QPen boundPen;
      boundPen.setStyle(Qt::DotLine);
      boundPen.setColor(Qt::black);

      painter->setPen(boundPen);
      painter->setBrush(QBrush());
      painter->drawRect(mBoundingRect);
   }   
}

//QMimeData *data = new QMimeData;
//data->setColorData(Qt::green);
//
//QDrag *drag = new QDrag(event->widget());
//drag->setMimeData(data);
//drag->start();
