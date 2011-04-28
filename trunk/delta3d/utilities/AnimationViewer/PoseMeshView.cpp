#include "PoseMeshView.h"
#include "PoseMeshScene.h"
#include "PoseMeshItem.h"
#include "ui_PoseMeshProperties.h"

#include <QtGui/QWheelEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsItem>
#include <QtGui/QPushButton>

#include <dtUtil/mathdefines.h>
#include <dtUtil/mswin.h>

#include <cassert>
#include <iostream>
#include <cmath>

const float kDefaultMaxScale = 6.0f;
const float kDefaultMinScale = 0.2f;

////////////////////////////////////////////////////////////////////////////////
PoseMeshView::PoseMeshView(PoseMeshScene* scene, QWidget* parent)
   : QGraphicsView(scene, parent)
   , mScene(scene)
   , mDragItem(NULL)
   , mMinScale(kDefaultMinScale)
   , mMaxScale(kDefaultMaxScale)
   , mCurrentScale(1.0f)
{
   centerOn(scene->sceneRect().center());

   // Make sure the point under the mouse remains unchanged when scaling
   setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

   mTimer.setParent(this);
   mTimer.setInterval(10);

   mCursor[MODE_GRAB]       = new QCursor(QPixmap(":/images/handIcon.png"));
   mCursor[MODE_BLEND_PICK] = new QCursor(QPixmap(":/images/reticle.png"));
   mCursor[MODE_ERROR_PICK] = new QCursor(QPixmap(":/images/epsilonCursor.png"), 0, 0);

   // Create and connect actions for the context menu
   mActionZoomItemExtents = new QAction("Zoom Extents", this);
   mActionClearBlend      = new QAction("Clear Blend", this);
   mActionToggleEnabled   = new QAction("Disable", this);
   mActionProperties      = new QAction("Properties", this);

   mPropertyContainer = new Ui::PoseMeshPropertiesContainer;
   mPropertyContainer->setupUi(&mPropertyDialog);

   // Connect the context menu actions
   connect(mActionZoomItemExtents, SIGNAL(triggered()), SLOT(OnZoomToItemExtents()));
   connect(mActionClearBlend, SIGNAL(triggered()), SLOT(OnClearBlend()));
   connect(mActionToggleEnabled, SIGNAL(triggered()), SLOT(OnToggleEnabled()));
   connect(mActionProperties, SIGNAL(triggered()), SLOT(OnShowProperties()));

   // Connect the dialog buttons
   connect(mPropertyContainer->buttonBox, SIGNAL(clicked(QAbstractButton*)),
           SLOT(OnPropertyDialogButtonPressed(QAbstractButton*)));

   // Allow our view to be frequently updated
   connect(&mTimer, SIGNAL(timeout()), this, SLOT(OnUpdateView()));
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshView::~PoseMeshView()
{

}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::wheelEvent(QWheelEvent* event)
{
   // Delta is in eighths of degrees
   int numberOfDegrees = event->delta() / 8;

   // Typical mice move in 15 degree steps
   float numberOfSteps = (float)numberOfDegrees / 15.0f;

   Zoom(numberOfSteps);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::keyPressEvent(QKeyEvent* event)
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

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::mouseMoveEvent(QMouseEvent* event)
{
   QGraphicsView::mouseMoveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::SetMode(eMODE newMode)
{
   bool allowMovement = (newMode == MODE_GRAB);

   QList<QGraphicsItem*> itemList = items();
   foreach (QGraphicsItem* item, itemList)
   {
      item->setFlag(QGraphicsItem::ItemIsMovable, allowMovement);
      item->setCursor(*mCursor[newMode]);
   }

   mMode = newMode;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::SetDisplayEdges(bool shouldDisplay)
{
   QList<QGraphicsItem*> itemList = items();
   foreach (QGraphicsItem* item, itemList)
   {
      PoseMeshItem *poseItem = dynamic_cast<PoseMeshItem*>(item);
      if (poseItem)
      {
         poseItem->SetDisplayEdges(shouldDisplay);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::SetDisplayError(bool shouldDisplay)
{
   QList<QGraphicsItem*> itemList = items();
   foreach (QGraphicsItem* item, itemList)
   {
      PoseMeshItem *poseItem = dynamic_cast<PoseMeshItem*>(item);
      if (poseItem)
      {
         poseItem->SetDisplayError(shouldDisplay);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshView::eMODE PoseMeshView::GetMode()
{
   return mMode;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::Zoom(float numberOfSteps)
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


   // Clamp the scale to our min/max
   dtUtil::Clamp(mCurrentScale, mMinScale, mMaxScale);

   QMatrix currentTransform = matrix();

   currentTransform.reset();
   currentTransform.scale(mCurrentScale, mCurrentScale);

   setMatrix(currentTransform);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnZoomToPoseMesh(const std::string& meshName)
{
   PoseMeshItem* item = mScene->GetPoseMeshItemByName(meshName);

   if (item)
   {
      // Set the scale so that when we zoom out,
      // it will happen from roughly the right place
      mCurrentScale = 2.0f;
      QMatrix currentTransform = matrix();

      currentTransform.reset();
      currentTransform.scale(mCurrentScale, mCurrentScale);

      setMatrix(currentTransform);

      QGraphicsView::fitInView(item, Qt::KeepAspectRatio);
   }
}

////////////////////////////////////////////////////////////////////////////////
float PoseMeshView::GetPercentVisible()
{
   QRect frameRect        = geometry();
   QRectF visibleRect     = mapToScene(frameRect).boundingRect();
   const QRectF totalRect = sceneRect();

   float visibleArea = visibleRect.width() * visibleRect.height();
   float totalArea   = totalRect.width() * totalRect.height();

   return visibleArea / totalArea;
}

////////////////////////////////////////////////////////////////////////////////
float PoseMeshView::GetScale()
{
   QMatrix currentTransform = matrix();

   float rightX = currentTransform.m11();
   float rightY = currentTransform.m12();

   // Return the magnitude of the right basis vector (Aka scale)
   return sqrt(rightX * rightX + rightY + rightY);
}


////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::setScene(QGraphicsScene* scene)
{
   QGraphicsView::setScene(scene);

   // Default this to the entire scene
   if (scene)
   {
      mItemRect = scene->sceneRect();
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::fitInView(const QRectF& rect, Qt::AspectRatioMode aspectRadioMode)
{
   QGraphicsView::fitInView(rect, aspectRadioMode);

   mCurrentScale = GetScale();

   mCurrentSource = rect.center();
   mCurrentTarget = mCurrentSource;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnSetCenterTarget(float sceneX, float sceneY)
{
   mCurrentTarget.setX(sceneX);
   mCurrentTarget.setY(sceneY);

   mTimer.start();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::mousePressEvent(QMouseEvent* event)
{
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

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::mouseReleaseEvent(QMouseEvent *event)
{
   //mDragItem = NULL;
   QGraphicsView::mouseReleaseEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::contextMenuEvent(QContextMenuEvent* event)
{
   QGraphicsView::contextMenuEvent(event);

   mLastItem = dynamic_cast<PoseMeshItem*>(itemAt(event->pos()));

   if (mLastItem)
   {
       QMenu menu;
       menu.addAction(mActionZoomItemExtents);

       // Set the correct toggle text
       if (mLastItem->isEnabled())
       {
          mActionToggleEnabled->setText("Disable");
       }
       else
       {
          mActionToggleEnabled->setText("Enable");
       }

        menu.addAction(mActionToggleEnabled);

       // Only necessary to clear items that are
       // currently blending poses
       if (mLastItem->IsActive())
       {
         menu.addAction(mActionClearBlend);
       }

       menu.addAction(mActionProperties);

       menu.exec(event->globalPos());
   }
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnZoomToItemExtents()
{
   OnZoomToPoseMesh(mLastItem->GetPoseMeshName());
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnClearBlend()
{
   mLastItem->Clear();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnToggleEnabled()
{
   if (mLastItem->isEnabled())
   {
      mLastItem->SetEnabled(false);
      mActionToggleEnabled->setText("Enable");
   }
   else
   {
      mLastItem->SetEnabled(true);
      mActionToggleEnabled->setText("Disable");
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnShowProperties()
{
   double minError = static_cast<double>(mLastItem->GetMinimumErrorValue());
   double maxError = static_cast<double>(mLastItem->GetMaximumErrorValue());

   mPropertyContainer->spinBoxMinimumPercentage->setValue(minError);
   mPropertyContainer->spinBoxMaximumPercentage->setValue(maxError);

   mPropertyDialog.show();
}



////////////////////////////////////////////////////////////////////////////////
void PoseMeshView::OnPropertyDialogButtonPressed(QAbstractButton* pressedButton)
{
   QAbstractButton* applyButton = mPropertyContainer->buttonBox->button(QDialogButtonBox::Apply);

   if (pressedButton == applyButton)
   {
      double minError = mPropertyContainer->spinBoxMinimumPercentage->value();
      double maxError = mPropertyContainer->spinBoxMaximumPercentage->value();

      mLastItem->SetMaximumErrorValue(static_cast<float>(maxError));
      mLastItem->SetMinimumErrorValue(static_cast<float>(minError));
   }
}
