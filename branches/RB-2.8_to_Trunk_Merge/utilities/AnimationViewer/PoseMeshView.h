#ifndef __POSE_MESH_VIEW_H__
#define __POSE_MESH_VIEW_H__

#include <QtGui/QGraphicsView>
#include <QtGui/QDialog>
#include <QtCore/QTimer>
#include "PoseMeshScene.h"

///////////////////////////////////////////////////////////////////////////////

class QAbstractButton;
class PoseMeshScene;
class PoseMeshItem;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class PoseMeshPropertiesContainer;
}
/// @endcond

///////////////////////////////////////////////////////////////////////////////

class PoseMeshView: public QGraphicsView
{
   Q_OBJECT
public:  
   
   enum eMODE
   {
      MODE_INVALID = -1,
      MODE_BLEND_PICK = 0,
      MODE_ERROR_PICK,  
      MODE_GRAB,

      MODE_TOTAL
   };

   PoseMeshView(PoseMeshScene* scene, QWidget* parent = 0);
   ~PoseMeshView();

   eMODE GetMode();
   void SetMode(eMODE newMode);   

   void Zoom(float numberOfSteps); 

   void SetDisplayEdges(bool shouldDisplay);
   void SetDisplayError(bool shouldDisplay);

   /// The item rect will constrain our view changed coordinates
   void SetItemBoundingRect(const QRectF& itemRect){ mItemRect = itemRect; }

   /// Return the percentage of the scene displayed
   float GetPercentVisible();

   // Assumes uniform scaling
   float GetScale();

   void SetMinScale(float minPercent) { mMinScale = minPercent; }
   void SetMaxScaled(float maxPercent){ mMaxScale = maxPercent; }   

   // Overrides
   void setScene(QGraphicsScene* scene);
   void fitInView(const QRectF& rect, Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);

public slots:
   void OnSetCenterTarget(float sceneX, float sceneY);  
   void OnUpdateView();
   void OnZoomToPoseMesh(const std::string &meshName);

protected:

   PoseMeshScene* mScene;
   PoseMeshItem*  mLastItem;
   QGraphicsItem* mDragItem;

   QPointF mCurrentTarget;
   QPointF mCurrentSource;
   QPointF mCurrentAcceleration;
   QPointF mCurrentVelocity;
   QRectF  mItemRect;

   QCursor* mCursor[MODE_TOTAL];

   QPointF mTestPoint;

   eMODE  mMode;

   float mMinScale;
   float mMaxScale;

   float mCurrentScale;

   // Event overrides
   void wheelEvent(QWheelEvent* event);
   void keyPressEvent(QKeyEvent* event);
   void mouseMoveEvent(QMouseEvent* event);  
   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);
   void contextMenuEvent(QContextMenuEvent* event);

private: 

   QPointF mLastClickPoint;
   QTimer  mTimer;
   bool    mIsMoving;

   QAction* mActionZoomItemExtents;
   QAction* mActionClearBlend;
   QAction* mActionToggleEnabled;
   QAction* mActionProperties;

   QDialog                          mPropertyDialog;
   Ui::PoseMeshPropertiesContainer* mPropertyContainer;

private slots:
   void OnZoomToItemExtents();
   void OnClearBlend();
   void OnToggleEnabled();
   void OnShowProperties();
   void OnPropertyDialogButtonPressed(QAbstractButton* pressedButton);
};

///////////////////////////////////////////////////////////////////////////////

#endif // _POSE_MESH_VIEW_H_
