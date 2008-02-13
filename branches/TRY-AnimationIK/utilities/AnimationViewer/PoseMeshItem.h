#ifndef _POSEMESH_ITEM_H_
#define _POSEMESH_ITEM_H_

#include <QtGui/QGraphicsRectItem>

class QGraphicsItem;
class QGraphicsScene;

namespace dtAnim { class PoseMesh; }

class PoseMeshItem: public QGraphicsItem
{   
public:
   PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                QGraphicsItem *parent = 0, 
                QGraphicsScene *scene = 0);

   ~PoseMeshItem(); 

   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

   QRectF boundingRect() const;
   QPainterPath shape() const;
   void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


protected: 

private: 

   QPixmap *mPixmap;


};

#endif // _POSEMESH_ITEM_H_
