#ifndef _POSEMESH_ITEM_H_
#define _POSEMESH_ITEM_H_

#include <dtAnim/PoseMesh.h>
#include <QtGui/QGraphicsRectItem>

class QGraphicsItem;
class QGraphicsScene;
class QCursor;

namespace dtAnim 
{
   class PoseMesh; 
   class CharDrawable;
}

class PoseMeshItem: public QGraphicsItem
{   
public:
   PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                dtAnim::CharDrawable *character,
                QGraphicsItem *parent = 0, 
                QGraphicsScene *scene = 0);

   ~PoseMeshItem(); 

   const std::string& GetPoseMeshName(); 

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
   dtAnim::CharDrawable *mCharacter;

   QPixmap *mPixmap;
   QRectF mBoundingRect;
   QPointF mLastMousePos;

   QCursor *mCursor;

   bool mHovered;

};

#endif // _POSEMESH_ITEM_H_
