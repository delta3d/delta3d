#ifndef _POSEMESH_ITEM_H_
#define _POSEMESH_ITEM_H_

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <QtGui/QGraphicsRectItem>

#include <set>

class QGraphicsItem;
class QGraphicsScene;
class QCursor;
class QAction;

namespace dtAnim { class Cal3DModelWrapper; }

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

class PoseMeshItem: public QGraphicsItem
{    
public:

   PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                dtAnim::Cal3DModelWrapper *model,
                QGraphicsItem *parent = NULL);

   ~PoseMeshItem(); 

   const std::string& GetPoseMeshName(); 

   void SetEnabled(bool isEnabled);
   bool IsActive();

   void SetDisplayErrorGrid(bool display);

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
   dtCore::RefPtr<dtAnim::PoseMeshUtility> mMeshUtil;

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
   bool    mIsErrorGridDisplayed;

   void BlendPosesFromItemCoordinates(float xCoord, float yCoord);

   void ExtractEdgesFromMesh(const dtAnim::PoseMesh &mesh);
   void ExtractErrorFromMesh(const dtAnim::PoseMesh &mesh);

   bool IsItemMovable();

   void PaintErrorSamples(QPainter *painter);  

   float GetErrorSample(const QPointF &samplePoint);

   QColor GetErrorColor(float degreesOfError);

   void GetTriangleBoundingRect(const dtAnim::PoseMesh::Triangle &tri, QRectF &outRect);

   bool GetIntersectionBoundingPoints(const QLineF &intersector, 
                                      const QLineF lines[3],
                                      QPointF &outLeftMost,
                                      QPointF &outRightMost);
};

#endif // _POSEMESH_ITEM_H_
