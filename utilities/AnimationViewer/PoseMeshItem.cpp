#include "PoseMeshItem.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneEvent>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QCursor>
#include <QtGui/QMenu>

#include <QtGui/QMouseEvent>
#include <QtGui/QHoverEvent>

#include <dtAnim/posemesh.h>
#include <dtAnim/posemath.h>
#include <dtAnim/chardrawable.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/refptr.h>

#include <osg/MatrixTransform>
#include <osg/LineWidth>
#include <osg/Geode>
#include <osg/Vec4>

#include <cassert>
#include <climits>

//temp
#include <sstream>
#include <iostream>

const float VERT_SCALE     = 100.0f;
const int VERT_RADIUS      = 6;
const int VERT_RADIUS_DIV2 = VERT_RADIUS / 2;

osg::Geometry* MakeLine(const osg::Vec3& beginPoint,
                        const osg::Vec3& endPoint,
                        const osg::Vec4& color,
                        const float size)
{
   osg::Vec3Array *vArray = new osg::Vec3Array(2);
   (*vArray)[0] = beginPoint;
   (*vArray)[1] = endPoint;

   osg::Vec4Array *colors = new osg::Vec4Array(1);
   (*colors)[0] = color;

   osg::Geometry* geometry = new osg::Geometry();
   geometry->setVertexArray(vArray);
   geometry->setColorArray(colors);
   geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
   geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

   osg::StateSet *dstate = new osg::StateSet();
   dstate->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   dstate->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

   osg::LineWidth *lineWidth = new osg::LineWidth(size);
   dstate->setAttribute(lineWidth);

   geometry->setStateSet(dstate);
   return geometry;
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshItem::PoseMeshItem(const dtAnim::PoseMesh& poseMesh,
                           dtAnim::CharDrawable* model,
                           QGraphicsItem* parent)
  : QGraphicsObject(parent)
  , mPoseMesh(&poseMesh)
  , mModel(model)
  , mBoundingRect()
  , mLastBlendPos(FLT_MAX, FLT_MAX)
  , mLastTriangleID(INT_MAX)
  , mIsActive(true)
  , mAreErrorSamplesDisplayed(false)
  , mAreEdgesDisplayed(true)
  , mErrorMinimum(0.0f)
  , mErrorMaximum(5.5f)
  , mScaleHoriz(1.0f)
  , mScaleVert(1.0f)
  , mShouldRecomputeError(false)
{
   assert(mModel);

   // We want to manually control when the item is movable
   setFlag(ItemIsMovable, false);

   // This needs to be set in order to receive mousemove events
   setFlag(ItemIsSelectable, true);

   // Make sure this gets drawn on top of the background
   setZValue(1);

   setAcceptsHoverEvents(true);
   setToolTip(poseMesh.GetName().c_str());

   mSampleCollection.mInitialized = false;

   mMeshUtil = new dtAnim::PoseMeshUtility;

   UpdateItemBoundingRect();

   // Get a unique set of edges and the
   // triangle they belong to
   ExtractEdgesFromMesh(*mPoseMesh);

   // Testing
   AssertZeroErrorAtVertices();
   AssertAzElConversion();
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshItem::~PoseMeshItem()
{

}

////////////////////////////////////////////////////////////////////////////////
const std::string& PoseMeshItem::GetPoseMeshName()
{
   return mPoseMesh->GetName();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetEnabled(bool isEnabled)
{
   setEnabled(isEnabled);
   Clear();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetDisplayEdges(bool shouldDisplay)
{
   mAreEdgesDisplayed = shouldDisplay;
   update();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetDisplayError(bool shouldDisplay)
{
   if (shouldDisplay && !mSampleCollection.mInitialized)
   {
      // If we haven't collected our samples, do so now
      ExtractErrorFromMesh(*mPoseMesh);
   }

   mAreErrorSamplesDisplayed = shouldDisplay;
   update();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::Clear()
{
   // Remove any of this item's pose blends from the model
   mMeshUtil->ClearPoses(mPoseMesh, mModel->GetModelWrapper(), 0.0f);

   mLastBlendPos.setX(FLT_MAX);
   mLastBlendPos.setY(FLT_MAX);
   mLastTriangleID = INT_MAX;

   // TODO - fix bone line rendering
   // Remove the line segments from the character view
   //RemoveBoneLinesFromScene();

   // Remove highlighting and target ellipse from scene
   update(boundingRect());
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetMinimumErrorValue(float minError)
{
   mErrorMinimum = minError;
   mShouldRecomputeError = true;
}

////////////////////////////////////////////////////////////////////////////////
float PoseMeshItem::GetMinimumErrorValue()
{
   return mErrorMinimum;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetMaximumErrorValue(float maxError)
{
   mErrorMaximum = maxError;
   mShouldRecomputeError = true;
}

////////////////////////////////////////////////////////////////////////////////
float PoseMeshItem::GetMaximumErrorValue()
{
   return mErrorMaximum;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetVerticalScale(float newScale)
{
   mScaleVert = newScale;
   UpdateItemBoundingRect();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetHorizontalScale(float newScale)
{
   mScaleHoriz = newScale;
   UpdateItemBoundingRect();
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::RemoveBoneLinesFromScene()
{
   // TODO - fix bone line rendering
   //osg::Geode *charGeode = dynamic_cast<osg::Geode*>(mModel->GetNode());
   //assert(charGeode);

   //charGeode->removeDrawable(mTrueLine.get());
   //charGeode->removeDrawable(mBlendLine.get());

   //mTrueLine = NULL;
   //mBlendLine = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::AddBoneLinesToScene(const dtAnim::PoseMesh::TargetTriangle& targetTri)
{
   // TODO - fix bone line rendering
   //osg::Geode *charGeode = dynamic_cast<osg::Geode*>(mModel->GetNode());
   //assert(charGeode);

   //// If lines already exist, remove them
   //if (mTrueLine.valid() || mBlendLine.valid())
   //{
   //   charGeode->removeDrawable(mTrueLine.get());
   //   charGeode->removeDrawable(mBlendLine.get());
   //}

   //osg::Vec3 trueDirection, blendDirection;
   //GetBoneDirections(targetTri, trueDirection, blendDirection);

   //trueDirection.normalize();
   //blendDirection.normalize();

   //osg::Vec3 baseForward;
   //GetAnchorBoneDirection(targetTri, baseForward);

   //float blendAzimuth, blendElevation;
   //dtAnim::GetCelestialCoordinates(blendDirection, baseForward, blendAzimuth, blendElevation);

   ////dtAnim::GetCelestialCoordinates(blendDirection, -osg::Y_AXIS, blendAzimuth, blendElevation);

   ///* std::ostringstream oss;
   //oss << "true value (" << trueDirection.x() << ", " << trueDirection.y() << ", " << trueDirection.z() << std::endl;
   //oss << "blend value (" << blendDirection.x() << ", " << blendDirection.y() << ", " << blendDirection.z() << std::endl;

   //std::cout << oss.str();*/

   //QColor errorColor = GetErrorColor(trueDirection, blendDirection);
   //osg::Vec4 osgColor(errorColor.redF(), errorColor.greenF(), errorColor.blueF(), 1.0f);

   //mModel->GetModelWrapper()->Update(0.0f);
   //osg::Vec3 startPos = mModel->GetModelWrapper()->GetBoneAbsoluteTranslation(mPoseMesh->GetBoneID());

   //mTrueLine  = MakeLine(startPos, startPos + trueDirection * 100.0f, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);
   //mBlendLine = MakeLine(startPos, startPos + blendDirection * 100.0f, osgColor, 1.0f);

   //charGeode->addDrawable(mTrueLine.get());
   //charGeode->addDrawable(mBlendLine.get());
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::OnBlendUpdate()
{
   // TODO - fix bone line rendering
   //// If bone lines exists, recreate them in their updated position
   //if (mTrueLine.valid() && mBlendLine.valid())
   //{
   //   // Scale back into pose space and flip the y coord
   //   dtAnim::PoseMesh::TargetTriangle targetTri;
   //   mPoseMesh->GetTargetTriangleData(mLastMousePos.x() / VERT_SCALE,
   //                                    -mLastMousePos.y() / VERT_SCALE, targetTri);

   //   // Only update when inside the tri
   //   if (targetTri.mIsInside)
   //   {
   //      AddBoneLinesToScene(targetTri);
   //   }
   //}
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::sceneEvent(QEvent* event)
{
   return QGraphicsItem::sceneEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
   if (!IsItemMovable())
   {
      if (event->button() == Qt::LeftButton)
      {
         // Convert the position back to it's unscaled form
         mLastMousePos = event->lastPos();

         BlendPosesFromItemCoordinates(mLastMousePos.x(), mLastMousePos.y());
      }
   }

   // Allow Qt to handle the other buttons
   QGraphicsItem::mousePressEvent(event);

}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
   if (!IsItemMovable() && (event->buttons() & Qt::LeftButton))
   {
      // Update our blend to the latest and greatest position
      mLastMousePos = event->pos();
      BlendPosesFromItemCoordinates(mLastMousePos.x(), mLastMousePos.y());
   }

   QGraphicsItem::mouseMoveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::BlendPosesFromItemCoordinates(float xCoord, float yCoord)
{
   float posemeshAzimuth   = xCoord / VERT_SCALE;
   float posemeshElevation = -yCoord / VERT_SCALE;

   posemeshAzimuth *= mScaleHoriz;
   posemeshElevation *= mScaleVert;

   // Scale back into pose space and flip the y coord
   dtAnim::PoseMesh::TargetTriangle targetTri;
   mPoseMesh->GetTargetTriangleData(posemeshAzimuth, posemeshElevation, targetTri);

   // Only update the blend and position if we're in the mesh
   if (targetTri.mIsInside)
   {
      dtAnim::BaseModelWrapper* modelWrapper = mModel->GetModelWrapper();
      mMeshUtil->BlendPoses(mPoseMesh, modelWrapper, targetTri, 0.0f);

      if (mAreErrorSamplesDisplayed)
      {
         // Make sure the skeleton is updated to the current
         // blend before we try to access it
         modelWrapper->UpdateAnimation(0.0f);

         AddBoneLinesToScene(targetTri);
      }

      // Store the last valid visual location of the blend
      mLastBlendPos.setX(xCoord);
      mLastBlendPos.setY(yCoord);
      mLastTriangleID = targetTri.mTriangleID;
   }

   // Make sure to redraw the changed portion
   update(boundingRect());

   emit NewItemBlend(mPoseMesh, posemeshAzimuth, posemeshElevation);
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::IsActive()
{
   return mIsActive;
}

////////////////////////////////////////////////////////////////////////////////
QRectF PoseMeshItem::boundingRect() const
{
   return mBoundingRect;
   //qreal adjust = 5;
   //return mBoundingRect.adjusted(adjust, adjust, adjust, adjust);
}

////////////////////////////////////////////////////////////////////////////////
QPainterPath PoseMeshItem::shape() const
{
   QPainterPath path;
   path.addRect(mBoundingRect);
   return path;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
   DTUNREFERENCED_PARAMETER(widget);
   DTUNREFERENCED_PARAMETER(option);

   if (mAreErrorSamplesDisplayed && isEnabled())
   {
      // If the min or max error range has change recompute should be true
      if (mShouldRecomputeError)
      {
         ExtractErrorFromMesh(*mPoseMesh);
         update();

         mShouldRecomputeError = false;
      }

      PaintErrorSamples(painter);
   }

   if (mAreEdgesDisplayed)
   {
      PaintEdges(painter);
   }

   const dtAnim::PoseMesh::VertexVector& verts = mPoseMesh->GetVertices();

   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      osg::Vec3 vertPosition = verts[vertIndex].mData * VERT_SCALE;
      vertPosition.x() *= mScaleHoriz;
      vertPosition.y() *= mScaleVert;

      QRadialGradient gradient(-3, -3, 10);
      if (isEnabled())
      {
         gradient.setColorAt(0, Qt::yellow);
         gradient.setColorAt(1, Qt::darkYellow);
      }
      else
      {
         gradient.setColorAt(0, Qt::gray);
         gradient.setColorAt(1, Qt::darkGray);
      }

      painter->translate(vertPosition.x(), -vertPosition.y());
      painter->setBrush(gradient);
      painter->setPen(QPen(Qt::black, 0));
      painter->drawEllipse(-VERT_RADIUS_DIV2, -VERT_RADIUS_DIV2, VERT_RADIUS, VERT_RADIUS);
      painter->translate(-vertPosition.x(), vertPosition.y());

      QPen boundPen;
      boundPen.setStyle(Qt::DotLine);
      boundPen.setColor(Qt::black);

      painter->setPen(boundPen);
      painter->setBrush(QBrush());
      painter->drawRect(mBoundingRect);

      // Translating by FLT_MAX is no bueno
      if (mLastBlendPos.x() != FLT_MAX)
      {
         QPen targetPen;
         targetPen.setStyle(Qt::DotLine);
         targetPen.setWidth(1);

         painter->translate(mLastBlendPos);
         painter->setPen(targetPen);
         painter->setBrush(QBrush(QColor(255, 255, 255), Qt::SolidPattern));
         painter->drawEllipse(-VERT_RADIUS_DIV2, -VERT_RADIUS_DIV2, VERT_RADIUS, VERT_RADIUS);
         painter->translate(-mLastBlendPos);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::PaintErrorSamples(QPainter* painter)
{
   for (size_t sampleIndex = 0; sampleIndex < mSampleCollection.mSamples.size(); ++sampleIndex)
   {
      QPointF samplePos = mSampleCollection.mSamples[sampleIndex].mSamplePosition;
      samplePos.rx() *= mScaleHoriz;
      samplePos.ry() *= mScaleVert;

      painter->translate(samplePos);
      //painter->setPen(QPen(mSampleCollection.mSamples[sampleIndex].mSampleColor, 0));
      painter->setPen(QPen(Qt::NoPen));
      painter->setBrush(mSampleCollection.mSamples[sampleIndex].mSampleColor);
      painter->drawRect(mSampleCollection.mSamples[sampleIndex].mSampleRect);
      painter->translate(-samplePos);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::PaintEdges(QPainter* painter)
{
   QPen trianglePenDefault;
   trianglePenDefault.setWidth(2);

   // Gray out the edges when the item is disabled
   if (isEnabled())
   {
      trianglePenDefault.setColor(Qt::black);
   }
   else
   {
      trianglePenDefault.setColor(QColor(128, 128, 128, 64));
   }

   // Draw edges connecting verts that are currently blended in white
   QPen trianglePenSelected;
   trianglePenSelected.setWidth(2);
   trianglePenSelected.setColor(Qt::white);

   for (size_t edgeIndex = 0; edgeIndex < mEdgeInfoList.size(); ++edgeIndex)
   {
      if (mEdgeInfoList[edgeIndex].triangleIDs[0] == mLastTriangleID ||
         mEdgeInfoList[edgeIndex].triangleIDs[1] == mLastTriangleID)
      {
         painter->setPen(trianglePenSelected);
      }
      else
      {
         painter->setPen(trianglePenDefault);
      }

      QPointF first  = mEdgeInfoList[edgeIndex].first;
      QPointF second = mEdgeInfoList[edgeIndex].second;

      first.rx() *= mScaleHoriz;
      first.ry() *= mScaleVert;

      second.rx() *= mScaleHoriz;
      second.ry() *= mScaleVert;

      painter->drawLine(first, second);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::ExtractEdgesFromMesh(const dtAnim::PoseMesh& mesh)
{
   typedef std::pair<dtAnim::PoseMesh::MeshIndexPair, int> VertIndexTriPair;
   typedef std::multimap<dtAnim::PoseMesh::MeshIndexPair, int> EdgeTriMap;

   EdgeTriMap edgeMap;

   const dtAnim::PoseMesh::TriangleVector& triList = mesh.GetTriangles();
   const dtAnim::PoseMesh::VertexVector& vertList = mesh.GetVertices();

   std::vector<dtAnim::PoseMesh::MeshIndexPair> edgeSet;

   // Go through each triangle and get a set of unique
   // edges and the triangles that contain them
   for (size_t triIndex = 0; triIndex < triList.size(); ++triIndex)
   {
      // Make 3 pairs of points to represent 3 edges of the tri
      dtAnim::PoseMesh::MeshIndexPair pair[3];
      mesh.GetIndexPairsForTriangle(triIndex, pair[0], pair[1], pair[2]);

      // Swapped pairs are considered equivalent, don't duplicate them
      dtAnim::PoseMesh::MeshIndexPair swapped[3];
      for (size_t pairIndex = 0; pairIndex < 3; ++pairIndex)
      {
         swapped[pairIndex].first  = pair[pairIndex].second;
         swapped[pairIndex].second = pair[pairIndex].first;
      }

      for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
      {
         EdgeTriMap::iterator pairIter = edgeMap.find(pair[edgeIndex]);
         EdgeTriMap::iterator swapIter = edgeMap.find(swapped[edgeIndex]);

         // Only use one ordering for each edge
         if (pairIter != edgeMap.end())
         {
            edgeMap.insert(VertIndexTriPair(pair[edgeIndex], triIndex));
         }
         else if (swapIter != edgeMap.end())
         {
            edgeMap.insert(VertIndexTriPair(swapped[edgeIndex], triIndex));
         }
         else
         {
            // Store the new edge that we haven't seen before
            edgeMap.insert(VertIndexTriPair(pair[edgeIndex], triIndex));

            // Store each unique edge when added for the first time
            // This is used to construct the final edge list vector
            edgeSet.push_back(pair[edgeIndex]);
         }
      }
   } // for each triangle

   // For each unique edge
   for (size_t setIndex = 0; setIndex < edgeSet.size(); ++setIndex)
   {
      dtAnim::PoseMesh::MeshIndexPair key = edgeSet[setIndex];
      EdgeTriMap::iterator rangeStart = edgeMap.find(key);
      EdgeTriMap::iterator rangeEnd   = edgeMap.upper_bound(key);

      assert(rangeStart != edgeMap.end());

      const dtAnim::PoseMesh::Vertex* vert0 = &vertList[key.first];
      const dtAnim::PoseMesh::Vertex* vert1 = &vertList[key.second];

      EdgeInfo newInfo;
      newInfo.first.rx()  = vert0->mData.x() * VERT_SCALE;
      newInfo.first.ry()  = vert0->mData.y() * -VERT_SCALE;
      newInfo.second.rx() = vert1->mData.x() * VERT_SCALE;
      newInfo.second.ry() = vert1->mData.y() * -VERT_SCALE;

      newInfo.triangleIDs[0] = rangeStart->second;

      if (++rangeStart != rangeEnd)
      {
         newInfo.triangleIDs[1] = rangeStart->second;
         //assert(++rangeStart == rangeEnd);
      }
      else
      {
         newInfo.triangleIDs[1] = -1;
      }

      mEdgeInfoList.push_back(newInfo);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::ExtractErrorFromMesh(const dtAnim::PoseMesh& mesh)
{
   dtAnim::BaseModelWrapper* modelWrapper = mModel->GetModelWrapper();
   assert(modelWrapper);

   // Make sure no animations are applied
   modelWrapper->ClearAllAnimations();
   modelWrapper->UpdateAnimation(0.0f);

   // Nothing is blended now so remove character lines
   RemoveBoneLinesFromScene();

   const dtAnim::PoseMesh::TriangleVector& triangleList = mesh.GetTriangles();

   // Sample points along a grid for every triangle
   for (size_t triIndex = 0; triIndex < triangleList.size(); ++triIndex)
   {
      dtAnim::PoseMesh::Triangle tri = triangleList[triIndex];

      osg::Vec3 point1 = tri.mVertices[0]->mData * VERT_SCALE;
      osg::Vec3 point2 = tri.mVertices[1]->mData * VERT_SCALE;
      osg::Vec3 point3 = tri.mVertices[2]->mData * VERT_SCALE;

      QLineF lines[3] =
      {
         QLineF(QPointF(point1.x(), point1.y()), QPointF(point2.x(), point2.y())),
         QLineF(QPointF(point2.x(), point2.y()), QPointF(point3.x(), point3.y())),
         QLineF(QPointF(point3.x(), point3.y()), QPointF(point1.x(), point1.y()))
      };

      QRectF triangleBounds;
      GetTriangleBoundingRect(tri, triangleBounds);

      QPointF gridOrigin(triangleBounds.left(), triangleBounds.bottom());
      gridOrigin.rx() -= 100.0f;

      const float dim = 3.0f;
      const float halfDim = dim * 0.5f;

      // Extend a horizontal line that will intersect the tri on the left and right
      QLineF horizGridLine(gridOrigin, gridOrigin + QPointF(1000.0f, 0.0f));

      QPointF leftBound, rightBound;

      while (horizGridLine.y2() < triangleBounds.top())
      {
         float verticalTranslation = halfDim;

         if (GetIntersectionBoundingPoints(horizGridLine, lines, leftBound, rightBound))
         {
            float sampleColumn = leftBound.x() + halfDim;

            while (sampleColumn < rightBound.x())
            {
               QPointF samplePos(sampleColumn, horizGridLine.y2());

               TriangleSample newSample;
               newSample.mSamplePosition.rx() = samplePos.x();
               newSample.mSamplePosition.ry() = -samplePos.y();
               newSample.mSampleRect.setLeft(-halfDim);
               newSample.mSampleRect.setTop(-halfDim);
               newSample.mSampleRect.setWidth(dim);
               newSample.mSampleRect.setHeight(dim);
               newSample.mError = GetErrorSample(samplePos);
               newSample.mSampleColor = GetErrorColor(newSample.mError);

               mSampleCollection.mSamples.push_back(newSample);

               sampleColumn += dim;
               verticalTranslation = dim;
            }
         }

         horizGridLine.translate(0.0f, verticalTranslation);
      }

      // Don't leave any leftover animations
      mMeshUtil->ClearPoses(mPoseMesh, modelWrapper, 0.0f);
   }

   // Mark this operation as complete
   mSampleCollection.mInitialized = true;
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::IsItemMovable()
{
   return flags() & QGraphicsItem::ItemIsMovable;
}

////////////////////////////////////////////////////////////////////////////////
float PoseMeshItem::GetErrorSample(const QPointF& samplePoint)
{
   dtAnim::BaseModelWrapper* modelWrapper = mModel->GetModelWrapper();
   assert(modelWrapper);

   QPointF meshSpaceTrueValue;
   meshSpaceTrueValue.rx() = samplePoint.x() / VERT_SCALE;
   meshSpaceTrueValue.ry() = samplePoint.y() / VERT_SCALE;

   dtAnim::PoseMesh::TargetTriangle blendTarget;
   mPoseMesh->GetTargetTriangleData(meshSpaceTrueValue.x(),
                                    meshSpaceTrueValue.y(),
                                    blendTarget);

   mMeshUtil->BlendPoses(mPoseMesh, modelWrapper, blendTarget, 0.0f);

   // Apply the blended pose for this sample
   modelWrapper->UpdateAnimation(0.0f);

   dtAnim::BoneInterface* rootBone = modelWrapper->GetBoneByIndex(mPoseMesh->GetRootID());
   dtAnim::BoneInterface* effectorBone = modelWrapper->GetBoneByIndex(mPoseMesh->GetEffectorID());

   osg::Quat effectorRotation = effectorBone->GetAbsoluteRotation();
   osg::Quat rootRotation = rootBone->GetAbsoluteRotation();

   // calculate a vector transformed by the rotation data.
   osg::Vec3 blendDirection = effectorRotation * mPoseMesh->GetEffectorForwardAxis();

   osg::Vec3 forwardDirection = rootRotation * mPoseMesh->GetRootForwardAxis();
   osg::Vec3 upDirection = osg::Z_AXIS;

   osg::Vec3 trueDirection;
   dtAnim::GetCelestialDirection(meshSpaceTrueValue.x(),
                                 meshSpaceTrueValue.y(),
                                 forwardDirection,
                                 upDirection,
                                 trueDirection);

   blendDirection.normalize();
   trueDirection.normalize();

   float blendDotTrue = blendDirection * trueDirection;
   dtUtil::Clamp(blendDotTrue, -1.0f, 1.0f);

   float radianAngle = acosf(blendDotTrue);
   return osg::RadiansToDegrees(radianAngle);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::GetBoneDirections(const dtAnim::PoseMesh::TargetTriangle& targetTri,
                                     osg::Vec3& outTrueDirection,
                                     osg::Vec3& outBlendDirection)
{
   dtAnim::BaseModelWrapper *modelWrapper = mModel->GetModelWrapper();
   dtAnim::BoneInterface* effectorBone = modelWrapper->GetBoneByIndex(mPoseMesh->GetEffectorID());
   osg::Quat boneRotation = effectorBone->GetAbsoluteRotation();

   // Get the direction that points forward for this pose mesh's bone
   const osg::Vec3& nativeBoneForward = mPoseMesh->GetEffectorForwardAxis();

   // calculate a vector transformed by the rotation data.
   outBlendDirection = boneRotation * nativeBoneForward;

   osg::Vec3 baseForward; // = -osg::Y_AXIS;
   GetAnchorBoneDirection(targetTri, baseForward);

   osg::Vec3 baseRight = osg::Z_AXIS ^ baseForward;
   osg::Vec3 baseUp = baseForward ^ baseRight;

   baseUp.normalize();


   dtAnim::GetCelestialDirection(targetTri.mAzimuth,
                                 targetTri.mElevation,
                                 baseForward,
                                 baseUp,
                                 outTrueDirection);
}

////////////////////////////////////////////////////////////////////////////////
// Warning, this function is expensive! (and experimental)
void PoseMeshItem::GetAnchorBoneDirection(const dtAnim::PoseMesh::TargetTriangle& currentTargetTri,
                                           osg::Vec3& outDirection)
{
   dtAnim::BaseModelWrapper* modelWrapper = mModel->GetModelWrapper();

   // Temp hack
   if (mPoseMesh->GetName() == "Poses_Gun")
   {
      osg::Quat boneRotation = modelWrapper->GetBoneByIndex(3)->GetAbsoluteRotation();
      osg::Quat test = modelWrapper->GetBoneByIndex(3)->GetRelativeRotation();
      outDirection = boneRotation * osg::Y_AXIS;

      osg::Vec3 asdf = test * -osg::Y_AXIS;
      outDirection = asdf;
   }
   //else if (mPoseMesh->GetName() == "Poses_Head")
   //{
   //   osg::Quat anchorRotation = modelWrapper->GetBoneAbsoluteRotation(7);
   //   osg::Quat anchorInverse = anchorRotation.inverse();

   //   osg::Vec3 test = anchorInverse * anchorRotation * -osg::Y_AXIS;

   //   osg::Quat effectorRotation = modelWrapper->GetBoneAbsoluteRotation(8);

   //   osg::Quat relativeRotation = modelWrapper->GetBoneRelativeRotation(8);
   //   osg::Quat relTest = anchorInverse * effectorRotation;

   //   osg::Vec3 newX = effectorRotation * osg::X_AXIS;
   //   osg::Vec3 newY = effectorRotation * osg::Y_AXIS;
   //   osg::Vec3 newZ = effectorRotation * osg::Z_AXIS;
   //
   //   osg::Vec3 asdf = (effectorRotation / anchorRotation) * -osg::Y_AXIS;
   //   outDirection = asdf;
   //}
   else
   {
      // Remove this mesh's contribution to the animation so we can get the baseline
      mMeshUtil->ClearPoses(mPoseMesh, mModel->GetModelWrapper(), 0.0f);

      // Apply the changes to the skeleton
      modelWrapper->UpdateAnimation(0.0f);

      // Get the bone's rotation without this pose mesh's animations applied
      osg::Quat boneRotation = modelWrapper->GetBoneByIndex(mPoseMesh->GetEffectorID())->GetAbsoluteRotation();

      const osg::Vec3& nativeBoneForward = mPoseMesh->GetEffectorForwardAxis();

      // Transform the native forward by base rotation
      outDirection = boneRotation * nativeBoneForward;

      // Re-apply the previous animation
      mMeshUtil->BlendPoses(mPoseMesh, mModel->GetModelWrapper(), currentTargetTri, 0.0f);
      modelWrapper->UpdateAnimation(0.0f);
   }
}

////////////////////////////////////////////////////////////////////////////////
QColor PoseMeshItem::GetErrorColor(float degreesOfError)
{
   QColor errorColor;

   float percentError = (degreesOfError - mErrorMinimum) / mErrorMaximum;
   dtUtil::Clamp(percentError, 0.0f, 1.0f);

   // In HSV color, red is at 0 and blue is at 240 degrees
   const float blue = 2.0f / 3.0f;
   errorColor.setHsvF(blue - percentError * blue, 1.0f, 1.0f);

   return errorColor;
}

////////////////////////////////////////////////////////////////////////////////
QColor PoseMeshItem::GetErrorColor(const osg::Vec3& first, const osg::Vec3& second)
{
   float firstDotSecond = first * second;
   dtUtil::Clamp(firstDotSecond, -1.0f, 1.0f);

   float angle = osg::RadiansToDegrees((acosf(firstDotSecond)));

   return GetErrorColor(angle);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::UpdateItemBoundingRect()
{
   // Clear out the old values
   mBoundingRect = QRectF();

   const dtAnim::PoseMesh::VertexVector &verts = mPoseMesh->GetVertices();

   // Determine the bounding box for this item
   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      osg::Vec3 vertPosition = verts[vertIndex].mData * VERT_SCALE;
      vertPosition.x() *= mScaleHoriz;
      vertPosition.y() *= mScaleVert;

      // Set the horizontal bounds
      if (vertPosition.x() < mBoundingRect.left())
      {
         mBoundingRect.setLeft(vertPosition.x());
      }
      else if (vertPosition.x() > mBoundingRect.right())
      {
         mBoundingRect.setRight(vertPosition.x());
      }

      // Set the vertical bounds
      if (-vertPosition.y() < mBoundingRect.top())
      {
         mBoundingRect.setTop(-vertPosition.y());
      }
      else if (-vertPosition.y() > mBoundingRect.bottom())
      {
         mBoundingRect.setBottom(-vertPosition.y());
      }
   }

   // Adjust the box to account for the vert radius
   mBoundingRect.adjust(-VERT_RADIUS_DIV2,
                        -VERT_RADIUS_DIV2,
                        VERT_RADIUS_DIV2,
                        VERT_RADIUS_DIV2);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::GetTriangleBoundingRect(const dtAnim::PoseMesh::Triangle& tri, QRectF& outRect)
{
   float left   = FLT_MAX;
   float right  = -FLT_MAX;
   float bottom = FLT_MAX;
   float top    = -FLT_MAX;

   for (size_t vertIndex = 0; vertIndex < 3; ++vertIndex)
   {
      const osg::Vec3& vertex = tri.mVertices[vertIndex]->mData;

      float x = vertex.x();
      float y = vertex.y();

      if (x < left)  { left   = x; }
      if (x > right) { right  = x; }
      if (y > top)   { top    = y; }
      if (y < bottom){ bottom = y; }
   }

   outRect.setLeft(left * VERT_SCALE);
   outRect.setRight(right * VERT_SCALE);
   outRect.setTop(top * VERT_SCALE);
   outRect.setBottom(bottom * VERT_SCALE);

   assert(left   != FLT_MAX);
   assert(right  != -FLT_MAX);
   assert(bottom != FLT_MAX);
   assert(top    != -FLT_MAX);
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::GetIntersectionBoundingPoints(const QLineF& intersector,
                                                 const QLineF lines[3],
                                                 QPointF& outLeftMost,
                                                 QPointF& outRightMost)
{
   bool foundLeft  = false;
   bool foundRight = false;

   outLeftMost.setX(FLT_MAX);
   outRightMost.setX(-FLT_MAX);

   // Collect the intersection points
   for (size_t lineIndex = 0; lineIndex < 3; ++lineIndex)
   {
      QPointF isectPoint;

      // If the lines intersect
      if (intersector.intersect(lines[lineIndex], &isectPoint) == QLineF::BoundedIntersection)
      {
         // Is this intersection the far left or far right?
         if (isectPoint.x() < outLeftMost.x())
         {
            outLeftMost = isectPoint;
            foundLeft = true;
         }

         if (isectPoint.x() > outRightMost.x())
         {
            outRightMost = isectPoint;
            foundRight = true;
         }
      }
   }

   return foundLeft & foundRight;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::AssertZeroErrorAtVertices()
{
   dtAnim::BaseModelWrapper* modelWrapper = mModel->GetModelWrapper();
   modelWrapper->ClearAllAnimations();
   modelWrapper->UpdateAnimation(0.0f);

   const dtAnim::PoseMesh::VertexVector& vertList = mPoseMesh->GetVertices();

   for (size_t vertIndex = 0; vertIndex < vertList.size(); ++vertIndex)
   {
      dtAnim::PoseMesh::TargetTriangle trianglePick;
      mPoseMesh->GetTargetTriangleData(vertList[vertIndex].mData.x(),
                                       vertList[vertIndex].mData.y(),
                                       trianglePick);

      //osg::Vec3 debugDirection = vertList[vertIndex]->mDebugData;
      //osg::Quat debugRotation  = vertList[vertIndex]->mDebugRotation;

      //int animID = vertList[vertIndex]->mAnimID;

      // Remove this mesh's contribution to the animation so we can get the baseline
      //mMeshUtil->ClearPoses(mPoseMesh, modelWrapper, 0.0f);
      modelWrapper->ClearAllAnimations();

      // Apply the changes to the skeleton
      modelWrapper->UpdateAnimation(0.0f);

      // Apply the animation to test
      mMeshUtil->BlendPoses(mPoseMesh, modelWrapper, trianglePick, 0.0f);

      // Apply new blend to skeleton
      modelWrapper->UpdateAnimation(0.0f);

      osg::Vec3 blendDirection;
      osg::Vec3 trueDirection;

      //GetBoneDirections(trianglePick, trueDirection, blendDirection);

      osg::Quat boneRotation = modelWrapper->GetBoneByIndex(mPoseMesh->GetEffectorID())->GetAbsoluteRotation();
         //osg::Quat boneRotation = modelWrapper->GetBoneAbsoluteRotationForKeyFrame(animID, mPoseMesh->GetBoneID(), 30);

         // Get the direction that points forward for this pose mesh's bone
         const osg::Vec3& nativeBoneForward = mPoseMesh->GetEffectorForwardAxis();

         // calculate a vector transformed by the rotation data.
         blendDirection = boneRotation * nativeBoneForward;

         osg::Vec3 baseForward = -osg::Y_AXIS;
         //GetAnchorBoneDirection(targetTri, baseForward);

         osg::Vec3 baseRight = osg::Z_AXIS ^ baseForward;
         osg::Vec3 baseUp = baseForward ^ baseRight;

         baseUp.normalize();

         //outTrueDirection = baseForward;
         dtAnim::GetCelestialDirection(trianglePick.mAzimuth,
                                       trianglePick.mElevation,
                                       baseForward,
                                       baseUp,
                                       trueDirection);

      trueDirection.normalize();
      blendDirection.normalize();

      float blendDotTrue = blendDirection * trueDirection;
      dtUtil::Clamp(blendDotTrue, -1.0f, 1.0f);

      //float angle = acosf(blendDotTrue);
      //assert(angle == 0.0f);
   }

   modelWrapper->ClearAllAnimations();
   modelWrapper->UpdateAnimation(0.0f);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::AssertAzElConversion()
{
   dtAnim::BaseModelWrapper* modelWrapper = mModel->GetModelWrapper();
   modelWrapper->ClearAllAnimations();
   modelWrapper->UpdateAnimation(0.0f);

   const dtAnim::PoseMesh::VertexVector &vertList = mPoseMesh->GetVertices();

   for (size_t vertIndex = 0; vertIndex < vertList.size(); ++vertIndex)
   {
      float azimuth = vertList[vertIndex].mData.x();
      float elevation = vertList[vertIndex].mData.y();

      osg::Vec3 debugDirection = vertList[vertIndex].mDebugData;
      //const osg::Quat& debugRotation  = vertList[vertIndex].mDebugRotation;

      //int animID = vertList[vertIndex]->mAnimID;

      osg::Vec3 directionFromAzEl;

      debugDirection.normalize();

      //outTrueDirection = baseForward;
      dtAnim::GetCelestialDirection(azimuth,
                                    elevation,
                                    -osg::Y_AXIS, // pose mesh uses this
                                    osg::Z_AXIS,
                                    directionFromAzEl);

      float newAzimuth, newElevation;
      dtAnim::GetCelestialCoordinates(directionFromAzEl, -osg::Y_AXIS, newAzimuth, newElevation);

      //directionFromAzEl = directionFromAzEl;
   }
}
