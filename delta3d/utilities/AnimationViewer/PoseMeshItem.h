#ifndef _POSEMESH_ITEM_H_
#define _POSEMESH_ITEM_H_

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/basemodelwrapper.h>
#include <QtGui/QGraphicsRectItem>

#include <set>

///////////////////////////////////////////////////////////////////////////////

class QCursor;
class QAction;

namespace dtAnim { class CharDrawable; }

struct TriangleSample
{
   osg::Vec3 mCoordinateTrue;
   osg::Vec3 mCoordinateSample;

   float mError;

   QPointF mSamplePosition;
   QRectF  mSampleRect;
   QColor  mSampleColor;
};

struct TriangleSampleSpace
{
   bool mInitialized;
   int mTriangleID;
   std::vector<TriangleSample> mSamples;
};

///////////////////////////////////////////////////////////////////////////////

class PoseMeshItem: public QGraphicsObject
{
   Q_OBJECT
public:

   PoseMeshItem(const dtAnim::PoseMesh& poseMesh,
                dtAnim::CharDrawable* model,
                QGraphicsItem* parent = NULL);

   ~PoseMeshItem();

   const std::string& GetPoseMeshName();

   void SetEnabled(bool isEnabled);
   void SetDisplayEdges(bool shouldDisplay);
   void SetDisplayError(bool shouldDisplay);
   bool IsActive();

   void OnBlendUpdate();

   void Clear();

   void  SetMinimumErrorValue(float minError);
   float GetMinimumErrorValue();

   void  SetMaximumErrorValue(float maxError);
   float GetMaximumErrorValue();

   void SetVerticalScale(float newScale);
   void SetHorizontalScale(float newScale);

   // Qt overrides
   virtual bool sceneEvent(QEvent* event);
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
   virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

   virtual QRectF boundingRect() const;
   virtual QPainterPath shape() const;
   virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

signals:
   void NewItemBlend(const dtAnim::PoseMesh* posemesh, float itemAzimth, float itemElevation);

private:

   const dtAnim::PoseMesh* mPoseMesh;
   dtAnim::CharDrawable* mModel;
   dtCore::RefPtr<dtAnim::PoseMeshUtility> mMeshUtil;

   // TODO - fix bone line rendering
   //osg::ref_ptr<osg::Geometry> mTrueLine;
   //osg::ref_ptr<osg::Geometry> mBlendLine;

   TriangleSampleSpace mSampleCollection;

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

   bool    mIsActive;
   bool    mAreErrorSamplesDisplayed;
   bool    mAreEdgesDisplayed;

   osg::Vec3 mBoneSpaceForward;

   float mErrorMinimum;
   float mErrorMaximum;
   float mScaleHoriz;
   float mScaleVert;
   bool  mShouldRecomputeError;

   void BlendPosesFromItemCoordinates(float xCoord, float yCoord);

   // Functions to precompute/extract data from the pose mesh
   void ExtractEdgesFromMesh(const dtAnim::PoseMesh& mesh);
   void ExtractErrorFromMesh(const dtAnim::PoseMesh& mesh);

   void GetBoneDirections(const dtAnim::PoseMesh::TargetTriangle& targetTri,
                          osg::Vec3& outTrueDirection,
                          osg::Vec3& outBlendDirection);

   void GetAnchorBoneDirection(const dtAnim::PoseMesh::TargetTriangle& currentTargetTri, osg::Vec3& outDirection);

   void AddBoneLinesToScene(const dtAnim::PoseMesh::TargetTriangle& targetTri);
   void RemoveBoneLinesFromScene();

   bool IsItemMovable();

   // Render functions
   void PaintErrorSamples(QPainter* painter);
   void PaintEdges(QPainter* painter);

   float GetErrorSample(const QPointF& samplePoint);

   QColor GetErrorColor(float degreesOfError);
   QColor GetErrorColor(const osg::Vec3& first, const osg::Vec3& second);

   void UpdateItemBoundingRect();

   void GetTriangleBoundingRect(const dtAnim::PoseMesh::Triangle& tri, QRectF& outRect);

   bool GetIntersectionBoundingPoints(const QLineF& intersector,
                                      const QLineF lines[3],
                                      QPointF& outLeftMost,
                                      QPointF& outRightMost);

   void AssertZeroErrorAtVertices();
   void AssertAzElConversion();
};

///////////////////////////////////////////////////////////////////////////////

#endif // _POSEMESH_ITEM_H_
