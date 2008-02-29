#ifndef _POSEMESH_ITEM_H_
#define _POSEMESH_ITEM_H_

#include <dtAnim/PoseMesh.h>
#include <QtGui/QGraphicsRectItem>

#include <set>

class QGraphicsItem;
class QGraphicsScene;
class QCursor;
class QAction;

namespace dtAnim { class Cal3DModelWrapper; }

class PoseMeshItem: public QGraphicsItem
{    
public:

   PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                dtAnim::Cal3DModelWrapper *model,
                QGraphicsItem *parent = 0, 
                QGraphicsScene *scene = 0);

   ~PoseMeshItem(); 

   const std::string& GetPoseMeshName(); 

   void SetEnabled(bool isEnabled);
   bool IsActive();

   void Clear();

   // Qt overrides
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
   dtAnim::Cal3DModelWrapper *mModel;   

   struct EdgeInfo
   {
      QPointF first;
      QPointF second;
      int     triangleIDs[2];
   };

   std::vector<EdgeInfo> mEdgeInfoList;

   QPixmap *mPixmap;
   QRectF mBoundingRect;  

   QPointF mLastMousePos;
   QPointF mLastBlendPos;
   int     mLastTriangleID;  

   void BlendPosesFromItemCoordinates(float xCoord, float yCoord);

   void ExtractEdgesFromMesh(const dtAnim::PoseMesh &mesh);

   bool IsItemMovable();
};

#endif // _POSEMESH_ITEM_H_
