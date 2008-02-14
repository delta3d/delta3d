#ifndef _TRACKVIEW_H_
#define _TRACKVIEW_H_

#include <QtGui/QGraphicsView>
#include <QtCore/QTimer>
#include "PoseMeshScene.h"

class PoseMeshScene;

class PoseMeshView: public QGraphicsView
{
   Q_OBJECT
public:  

   PoseMeshView(PoseMeshScene *scene, QWidget *parent = 0);
   ~PoseMeshView();

   void Zoom(float numberOfSteps, QPoint centerPoint = QPoint());

   /// The item rect will constrain our view changed coordinates
   void SetItemBoundingRect(const QRectF &itemRect){ mItemRect = itemRect; }

   /// Return the percentage of the scene displayed
   float GetPercentVisible();

   // Assumes uniform scaling
   float GetScale();

   void SetMinScale(float minPercent) { mMinScale = minPercent; }
   void SetMaxScaled(float maxPercent){ mMaxScale = maxPercent; }   

   // Overrides
   void setScene(QGraphicsScene *scene);
   void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);

public slots:
   void OnSetCenterTarget(float sceneX, float sceneY);  
   void OnUpdateView();

protected:

   PoseMeshScene *mScene;
   QGraphicsItem *mDragItem;

   QPointF mCurrentTarget;
   QPointF mCurrentSource;
   QPointF mCurrentAcceleration;
   QPointF mCurrentVelocity;
   QRectF  mItemRect;

   QPointF mTestPoint;

   float mMinScale;
   float mMaxScale;

   float mCurrentScale;

   // Event overrides
   void wheelEvent(QWheelEvent *event);
   void keyPressEvent(QKeyEvent *event);
   void mouseMoveEvent(QMouseEvent *event);  
   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void contextMenuEvent(QContextMenuEvent *event);

private: 

   QPointF mLastClickPoint;
   QTimer  mTimer;
   bool    mIsMoving;

};

#endif
