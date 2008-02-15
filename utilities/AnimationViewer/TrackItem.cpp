#include "TrackItem.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneEvent>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QImage>


TrackItem::TrackItem(QGraphicsItem *parent, QGraphicsScene *scene)
{
   QSize size(40, 20);
   QImage image(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
   image.fill(qRgba(0, 0, 0, 0));

   QFont font;
   font.setStyleStrategy(QFont::ForceOutline);

   QPainter painter;
   painter.begin(&image);
   painter.setRenderHint(QPainter::Antialiasing);
   painter.setBrush(Qt::white);
   painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1), 25, 25);

   //painter.setFont(font);
   //painter.setBrush(Qt::black);
   //painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
   painter.end();

   mPixmap = new QPixmap;
   *mPixmap = QPixmap::fromImage(image);
}


TrackItem::~TrackItem()
{

}


void TrackItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{  
   QMimeData *data = new QMimeData;
   data->setColorData(Qt::green);
   
   QDrag *drag = new QDrag(event->widget());
   drag->setMimeData(data);
   drag->setPixmap(*mPixmap);   
   drag->start(Qt::MoveAction);
   show();  
}

void TrackItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

//QMimeData *data = new QMimeData;
//data->setColorData(Qt::green);
//
//QDrag *drag = new QDrag(event->widget());
//drag->setMimeData(data);
//drag->start();
