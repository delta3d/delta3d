#ifndef _TRACKITEM_H_
#define _TRACKITEM_H_

#include <QtGui/QGraphicsRectItem>

class QGraphicsItem;
class QGraphicsScene;

class TrackItem: public QGraphicsRectItem
{   
public:
   TrackItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
   ~TrackItem(); 

   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected: 

private: 

   QPixmap *mPixmap;


};

#endif
