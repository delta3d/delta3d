#include "PoseMeshView.h"
#include "PoseMeshScene.h"
#include "PoseMeshItem.h"

#include <QtGui/QWheelEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsItem>

#include <dtUtil/mathdefines.h>

#include <cassert>
#include <iostream>
#include <cmath>

const float kDefaultMaxScale = 6.0f;
const float kDefaultMinScale = 0.2f;


PoseMeshView::PoseMeshView(PoseMeshScene *scene, QWidget *parent)
:QGraphicsView(scene, parent) 
, mScene(scene)
, mDragItem(NULL)
, mMinScale(kDefaultMinScale)
, mMaxScale(kDefaultMaxScale)
, mCurrentScale(1.0f)
{   
   centerOn(scene->sceneRect().center());     
   Zoom(10);   
 
   mTimer.setParent(this);
   mTimer.setInterval(50);  

   setAcceptDrops(true);

   //mPoseMeshViewer->setDragMode(QGraphicsView::ScrollHandDrag);

   connect(&mTimer, SIGNAL(timeout()), this, SLOT(OnUpdateView()));   
}


PoseMeshView::~PoseMeshView()
{
   
}



void PoseMeshView::wheelEvent(QWheelEvent *event)
{
   // Delta is in eighths of degrees
   int numberOfDegrees = event->delta() / 8;

   // Typical mice move in 15 degree steps
   float numberOfSteps = (float)numberOfDegrees / 15.0f;

   Zoom(numberOfSteps, event->pos());
}


void PoseMeshView::keyPressEvent(QKeyEvent *event)
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


void PoseMeshView::mouseMoveEvent(QMouseEvent *event)
{
   QGraphicsView::mouseMoveEvent(event);   
}


void PoseMeshView::Zoom(float numberOfSteps, QPoint centerPoint)
{   
   const float kScaleFactor = 1.075f;
   float scaleAmount = 0;

   if (numberOfSteps > 0)
   {    
      float adjustedFactor = 0.075f * mCurrentScale / mMaxScale;
      scaleAmount = numberOfSteps * (kScaleFactor - adjustedFactor);
   }
   else
   {     
      // numberOfSteps is negative here
      scaleAmount = -1.0f / (numberOfSteps * kScaleFactor);      
   }

   mCurrentScale *= scaleAmount;

   QPointF sceneCenter = mapToScene(centerPoint);

   QGraphicsItem *centerItem = mScene->itemAt(sceneCenter);

   if (centerItem && centerItem->isEnabled())
   {
      centerOn(centerItem);
   }
   else
   {
      centerOn(sceneCenter);
   }

   // Clamp the scale to our min/max
   dtUtil::Clamp(mCurrentScale, mMinScale, mMaxScale);

   QMatrix currentTransform = matrix(); 

   currentTransform.reset();    
   currentTransform.scale(mCurrentScale, mCurrentScale);

   setMatrix(currentTransform);     
}

void PoseMeshView::OnZoomToPoseMesh(const std::string &meshName)
{
   PoseMeshItem *item = mScene->GetPoseMeshItemByName(meshName);

   if (item)
   {      
      QGraphicsView::fitInView(item, Qt::KeepAspectRatio);
   }
}


float PoseMeshView::GetPercentVisible()
{
   QRect frameRect        = geometry();
   QRectF visibleRect     = mapToScene(frameRect).boundingRect();
   const QRectF totalRect = sceneRect();

   float visibleArea = visibleRect.width() * visibleRect.height();
   float totalArea   = totalRect.width() * totalRect.height();

   return visibleArea / totalArea;
}


float PoseMeshView::GetScale()
{
   QMatrix currentTransform = matrix();

   float rightX = currentTransform.m11();
   float rightY = currentTransform.m12();   

   // Return the magnitude of the right basis vector (Aka scale)
   return sqrt(rightX * rightX + rightY + rightY);
}



void PoseMeshView::setScene(QGraphicsScene *scene)
{
   QGraphicsView::setScene(scene);

   // Default this to the entire scene
   if (scene)
   {
      mItemRect = scene->sceneRect();
   }   
}

void PoseMeshView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode)
{
   QGraphicsView::fitInView(rect, aspectRadioMode);   

   mCurrentScale = GetScale();     

   mCurrentSource = rect.center();
   mCurrentTarget = mCurrentSource;   
}

void PoseMeshView::OnSetCenterTarget(float sceneX, float sceneY)
{      
   mCurrentTarget.setX(sceneX);
   mCurrentTarget.setY(sceneY);  

   mTimer.start();        
}

void PoseMeshView::mousePressEvent(QMouseEvent *event)
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


void PoseMeshView::mouseReleaseEvent(QMouseEvent *event)
{
   //mDragItem = NULL;
   QGraphicsView::mouseReleaseEvent(event);
   //setDragMode(QGraphicsView::ScrollHandDrag);
}


void PoseMeshView::contextMenuEvent( QContextMenuEvent *event )
{
   mLastClickPoint = mapToScene(event->pos());

   QMenu menu;
   
   //const QGraphicsItem *item = itemAt(event->pos()); 

   menu.exec(event->globalPos());
}


void PoseMeshView::OnUpdateView()
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
