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

   virtual bool sceneEvent(QEvent *event);
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
   virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
   
   virtual QRectF boundingRect() const;
   virtual QPainterPath shape() const;
   virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


protected: 

private: 

   const dtAnim::PoseMesh *mPoseMesh;
   QPixmap *mPixmap;
   QRectF mBoundingRect;
   QPointF mLastMousePos;

   bool mHovered;


};

#endif // _POSEMESH_ITEM_H_
