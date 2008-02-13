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


PoseMeshItem::PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                           QGraphicsItem *parent, 
                           QGraphicsScene *scene)
{
   setFlag(ItemIsMovable);
   setZValue(1);
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
}


PoseMeshItem::~PoseMeshItem()
{

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

QRectF PoseMeshItem::boundingRect() const
{
   qreal adjust = 2;
   return QRectF(-10 - adjust, -10 - adjust,
                 23 + adjust, 23 + adjust);
}

QPainterPath PoseMeshItem::shape() const
{
   QPainterPath path;
   path.addEllipse(-7, -7, 14, 14);
   return path;
}

void PoseMeshItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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
      gradient.setColorAt(0, Qt::yellow);
      gradient.setColorAt(1, Qt::darkYellow);
   }

   painter->setBrush(gradient);
   painter->setPen(QPen(Qt::black, 0));
   painter->drawEllipse(-7, -7, 14, 14);
}

//QMimeData *data = new QMimeData;
//data->setColorData(Qt::green);
//
//QDrag *drag = new QDrag(event->widget());
//drag->setMimeData(data);
//drag->start();
