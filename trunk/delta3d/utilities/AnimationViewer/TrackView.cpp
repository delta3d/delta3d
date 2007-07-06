#include "TrackView.h"
#include "TrackScene.h"

#include <QtGui/QWheelEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsItem>

#include <cassert>
#include <iostream>
#include <cmath>

const float kDefaultMaxScale = 6.0f;
const float kDefaultMinScale = 0.2f;


TrackView::TrackView(TrackScene *scene, QWidget *parent)
:QGraphicsView(scene, parent) 
, mMaxScale(kDefaultMaxScale)
, mMinScale(kDefaultMinScale)
, mCurrentScale(1.0f)
, mScene(scene)
, mDragItem(NULL)
{   
   centerOn(scene->sceneRect().center());     
   Zoom(10);   
 
   mTimer.setParent(this);
   mTimer.setInterval(50);  

   setAcceptDrops(true);

   connect(&mTimer, SIGNAL(timeout()), this, SLOT(OnUpdateView()));   
}


TrackView::~TrackView()
{
   
}



void TrackView::wheelEvent(QWheelEvent *event)
{
   // Delta is in eighths of degrees
   int numberOfDegrees = event->delta() / 8;

   // Typical mice move in 15 degree steps
   float numberOfSteps = (float)numberOfDegrees / 15.0f;

   Zoom(numberOfSteps);
}


void TrackView::keyPressEvent(QKeyEvent *event)
{
   QGraphicsView::keyPressEvent(event);

   if (event->key() == '+')
   {
      Zoom(1.0f);
   }
   else if (event->key() == '-')
   {
      Zoom(-1.0f);
   }
}


void TrackView::mouseMoveEvent(QMouseEvent *event)
{
   OnViewChanged();
   QGraphicsView::mouseMoveEvent(event);   
}


void TrackView::Zoom(float numberOfSteps)
{   
   const float kScaleFactor = 1.1f;
   float scaleAmount = 0;

   if (numberOfSteps > 0)
   {       
      scaleAmount = numberOfSteps * kScaleFactor;     
   }
   else
   {     
      // numberOfSteps is negative here
      scaleAmount = -1.0f / (numberOfSteps * kScaleFactor);      
   }

   mCurrentScale *= scaleAmount;

   // Clamp the scale to our min/max
   if (mCurrentScale > mMaxScale)
   {
      mCurrentScale = mMaxScale;
   }
   else if (mCurrentScale < mMinScale)
   {
      mCurrentScale = mMinScale;
   }

   QMatrix currentTransform = matrix();   

   currentTransform.reset();      
   currentTransform.scale(mCurrentScale, mCurrentScale);

   setMatrix(currentTransform);   

   // The pushpins need to know this
   emit MapScaleChanged(mCurrentScale);

   OnViewChanged();   
}


float TrackView::GetPercentVisible()
{
   QRect frameRect        = geometry();
   QRectF visibleRect     = mapToScene(frameRect).boundingRect();
   const QRectF totalRect = sceneRect();

   float visibleArea = visibleRect.width() * visibleRect.height();
   float totalArea   = totalRect.width() * totalRect.height();

   return visibleArea / totalArea;
}


float TrackView::GetScale()
{
   QMatrix currentTransform = matrix();

   float rightX = currentTransform.m11();
   float rightY = currentTransform.m12();   

   // Return the magnitude of the right basis vector (Aka scale)
   return sqrt(rightX * rightX + rightY + rightY);
}



void TrackView::setScene(QGraphicsScene *scene)
{
   QGraphicsView::setScene(scene);

   // Default this to the entire scene
   if (scene)
   {
      mItemRect = scene->sceneRect();
   }   
}


void TrackView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode)
{
   QGraphicsView::fitInView(rect, aspectRadioMode);   

   mCurrentScale = GetScale();

   // The pushpins need to know this
   emit MapScaleChanged(mCurrentScale);  

   OnViewChanged();

   mCurrentSource = rect.center();
   mCurrentTarget = mCurrentSource;   
}


void TrackView::OnSetCenterTarget(float sceneX, float sceneY)
{      
   mCurrentTarget.setX(sceneX);
   mCurrentTarget.setY(sceneY);  

   mTimer.start();        
}


void TrackView::OnViewChanged()
{
   QRect geom = rect();
   QRectF viewRect = mapToScene(geom).boundingRect();   

   // This will prevent the viewRect from going outside the bounds of the item rect
   QRectF constrainedRect = mItemRect.intersected(viewRect);

   float top    = (constrainedRect.top() - mItemRect.top()) / mItemRect.height();
   float left   = (constrainedRect.left() - mItemRect.left()) / mItemRect.width();
   float bottom = top + constrainedRect.height() / mItemRect.height();
   float right  = left + constrainedRect.width() / mItemRect.width();

   QRectF zeroToOneRect;
   zeroToOneRect.setTop(top);
   zeroToOneRect.setLeft(left);
   zeroToOneRect.setRight(right);
   zeroToOneRect.setBottom(bottom);

   mCurrentSource = mapToScene(geom.center());
   mCurrentTarget = mCurrentSource;  

   emit ViewChanged(zeroToOneRect);
}


void TrackView::mousePressEvent(QMouseEvent *event)
{
   //std::vector<AnimElement>* elements = NULL;
   //mScene->GetAnimElements(elements);
   //assert(elements);
  
   //for (int elemIndex = 0; elemIndex < elements->size(); ++elemIndex)
   //{
   //   QPointF testPos = event->pos();
   //   const QPointF pos = mapToScene(event->pos());

   //   if ((*elements)[elemIndex].mGraphicsItem->contains(pos))
   //   {
   //      mTestPoint = pos;
   //      mDragItem = (*elements)[elemIndex].mGraphicsItem;
   //      setDragMode(QGraphicsView::NoDrag);
   //   }
   //}   

   if (mIsMoving)
   {
      if (event->button() & Qt::LeftButton)
      {
         QPointF newP = mapToScene(event->pos());        
         mIsMoving = false;
      }
   }

   // Stop interpolation 
   mTimer.stop();

   QGraphicsView::mousePressEvent(event);
}


void TrackView::mouseReleaseEvent(QMouseEvent *event)
{
   //mDragItem = NULL;
   QGraphicsView::mouseReleaseEvent(event);
   setDragMode(QGraphicsView::ScrollHandDrag);
}


void TrackView::contextMenuEvent( QContextMenuEvent *event )
{
   mLastClickPoint = mapToScene(event->pos());

   QMenu menu;
   
   //const QGraphicsItem *item = itemAt(event->pos()); 

   menu.exec(event->globalPos());
}


void TrackView::OnUpdateView()
{
   QPointF direction = mCurrentTarget - mCurrentSource;

   if ((direction.x() * direction.x() + direction.y() * direction.y()) > 1.0f)
   {
      QLineF directionLine = QLineF(0.0f, 0.0f, direction.x(), direction.y()).unitVector();

      mCurrentSource += direction * 0.25f;
      centerOn(mCurrentSource);
   }  
   else
   {
      mCurrentSource = mCurrentTarget;

      centerOn(mCurrentTarget);
      mTimer.stop();
   }
}
