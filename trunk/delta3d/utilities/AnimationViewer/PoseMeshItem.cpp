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

#include <assert.h>

//temp
#include <sstream>
#include <iostream>

const float VERT_SCALE     = 100.0f;
const int VERT_RADIUS      = 6;
const int VERT_RADIUS_DIV2 = VERT_RADIUS / 2;

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshItem::PoseMeshItem(const dtAnim::PoseMesh &poseMesh,
                           dtAnim::Cal3DModelWrapper *model,
                           QGraphicsItem *parent)
  : QGraphicsItem(parent)
  , mPoseMesh(&poseMesh)
  , mModel(model)
  , mBoundingRect()
  , mLastBlendPos(FLT_MAX, FLT_MAX)
  , mLastTriangleID(INT_MAX)
  , mIsActive(true)
  , mIsErrorGridDisplayed(false)
{
   assert(mModel);

   // We want to manually control when the item is movable
   setFlag(ItemIsMovable, false);

   // Make sure this gets drawn on top of the background
   setZValue(1);

   setAcceptsHoverEvents(true);
   setToolTip(poseMesh.GetName().c_str());     

   mSampleCollection.mInitialized = false;

   mMeshUtil = new dtAnim::PoseMeshUtility;
   
   //QSize size(40, 20);
   //QImage image(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
   //image.fill(qRgba(0, 0, 0, 0));

   //QFont font;
   //font.setStyleStrategy(QFont::ForceOutline);

   //QPainter painter;
   //painter.begin(&image);
   //painter.setRenderHint(QPainter::Antialiasing);
   //painter.setBrush(Qt::white);
   //painter.drawRoundRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1), 25, 25);

   ////painter.setFont(font);
   ////painter.setBrush(Qt::black);
   ////painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
   //painter.end();

   //mPixmap = new QPixmap;
   //*mPixmap = QPixmap::fromImage(image);

   const dtAnim::PoseMesh::VertexVector &verts = mPoseMesh->GetVertices();

   // Determine the bounding box for this item
   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      osg::Vec3 vertPosition = verts[vertIndex]->mData * VERT_SCALE;      

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

   // Get a unique set of edges and the 
   // triangle they belong to
   ExtractEdgesFromMesh(*mPoseMesh);  
}

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshItem::~PoseMeshItem()
{

}

/////////////////////////////////////////////////////////////////////////////////////////
const std::string& PoseMeshItem::GetPoseMeshName()
{
   return mPoseMesh->GetName(); 
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetEnabled(bool isEnabled)
{
   setEnabled(isEnabled);  
   Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::SetDisplayErrorGrid(bool display)
{
   if (display && !mSampleCollection.mInitialized)
   {
      // If we haven't collected our samples, do so now
      ExtractErrorFromMesh(*mPoseMesh);
   }

   mIsErrorGridDisplayed = display;   
   update();
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::Clear()
{
   // Remove any of this item's pose blends from the model   
   mMeshUtil->ClearPoses(mPoseMesh, mModel, 0.0f);

   mLastBlendPos.setX(FLT_MAX);
   mLastBlendPos.setY(FLT_MAX);
   mLastTriangleID = INT_MAX;

   // Remove highlighting and target ellipse from scene
   update(boundingRect());
}

/////////////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::sceneEvent(QEvent *event)
{
   //std::ostringstream oss;
   //oss << "event: " << event->type();
   //std::cout << oss.str() << std::endl;

   return QGraphicsItem::sceneEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{   
   if (!IsItemMovable())
   {
      if (event->button() == Qt::LeftButton)
      {     
         // Convert the position back to it's unscaled form
         mLastMousePos = event->lastPos(); 

         //std::ostringstream oss;
         //oss << "pos: (" << mLastMousePos.x() << ", " << mLastMousePos.y() << ")";
         //std::cout << oss.str() << std::endl;

         BlendPosesFromItemCoordinates(mLastMousePos.x(), mLastMousePos.y());     
      }  
   }   
  
   // Allow Qt to handle the other buttons
   QGraphicsItem::mousePressEvent(event);
   
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
   if (!IsItemMovable())
   {
      // Update our blend to the latest and greatest position
      mLastMousePos = event->pos();
      BlendPosesFromItemCoordinates(mLastMousePos.x(), mLastMousePos.y());
   }  

   QGraphicsItem::mouseMoveEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::BlendPosesFromItemCoordinates(float xCoord, float yCoord)
{
   // Scale back into pose space and flip the y coord
   dtAnim::PoseMesh::TargetTriangle targetTri;
   mPoseMesh->GetTargetTriangleData(xCoord / VERT_SCALE, -yCoord / VERT_SCALE, targetTri);

   // Only update the blend and position if we're in the mesh
   if (targetTri.mIsInside)
   { 
      mMeshUtil->BlendPoses(mPoseMesh, mModel, targetTri);

      //std::ostringstream oss;
      //oss << "x = " << xCoord << "  " << "y = " << yCoord 
      //    << "  "   << "tri = " << targetTri.mTriangleID;

      //std::cout << oss.str() << std::endl;

      // Store the last valie visual location of the blend
      mLastBlendPos.setX(xCoord);
      mLastBlendPos.setY(yCoord);
      mLastTriangleID = targetTri.mTriangleID;
   }   

   // Make sure to redraw the changed portion
   update(boundingRect());
}

/////////////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::IsActive()
{   
   return mIsActive;
}

/////////////////////////////////////////////////////////////////////////////////////////
QRectF PoseMeshItem::boundingRect() const
{
   return mBoundingRect;
   //qreal adjust = 5;
   //return mBoundingRect.adjusted(adjust, adjust, adjust, adjust);
}

/////////////////////////////////////////////////////////////////////////////////////////
QPainterPath PoseMeshItem::shape() const
{
   QPainterPath path;   
   path.addRect(mBoundingRect);
   return path;
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   PaintErrorSamples(painter);

   const dtAnim::PoseMesh::VertexVector &verts = mPoseMesh->GetVertices();

   QPen trianglePenDefault;  
   trianglePenDefault.setWidth(2);

   if (isEnabled())
   {
      trianglePenDefault.setColor(Qt::black);
   }
   else
   {
      trianglePenDefault.setColor(QColor(128, 128, 128, 64));
   }

   QPen trianglePenSelected;
   trianglePenSelected.setWidth(2);
   trianglePenSelected.setColor(Qt::green);

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

      painter->drawLine(mEdgeInfoList[edgeIndex].first, mEdgeInfoList[edgeIndex].second);
   }   

   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      osg::Vec3 vertPosition = verts[vertIndex]->mData * VERT_SCALE;

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

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::PaintErrorSamples(QPainter *painter)
{
   for (size_t sampleIndex = 0; sampleIndex < mSampleCollection.mSamples.size(); ++sampleIndex)
   {
      painter->translate(mSampleCollection.mSamples[sampleIndex].mSamplePosition);     
      //painter->setPen(QPen(mSampleCollection.mSamples[sampleIndex].mSampleColor, 0));
      painter->setPen(QPen(Qt::NoPen));
      painter->setBrush(mSampleCollection.mSamples[sampleIndex].mSampleColor);
      painter->drawRect(mSampleCollection.mSamples[sampleIndex].mSampleRect);
      painter->translate(-mSampleCollection.mSamples[sampleIndex].mSamplePosition);
   }   
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::ExtractEdgesFromMesh(const dtAnim::PoseMesh &mesh)
{
   typedef std::pair<dtAnim::PoseMesh::MeshIndexPair, int> VertIndexTriPair;
   typedef std::multimap<dtAnim::PoseMesh::MeshIndexPair, int> EdgeTriMap;

   EdgeTriMap edgeMap;

   const dtAnim::PoseMesh::TriangleVector &triList = mesh.GetTriangles();
   const dtAnim::PoseMesh::VertexVector &vertList = mesh.GetVertices();  

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

      dtAnim::PoseMesh::Vertex *vert0 = vertList[key.first];
      dtAnim::PoseMesh::Vertex *vert1 = vertList[key.second];

      EdgeInfo newInfo;
      newInfo.first.rx()  = vert0->mData.x() * VERT_SCALE;
      newInfo.first.ry()  = vert0->mData.y() * -VERT_SCALE;  
      newInfo.second.rx() = vert1->mData.x() * VERT_SCALE;
      newInfo.second.ry() = vert1->mData.y() * -VERT_SCALE;  

      newInfo.triangleIDs[0] = rangeStart->second;

      if (++rangeStart != rangeEnd)
      {
         newInfo.triangleIDs[1] = rangeStart->second;
         assert(++rangeStart == rangeEnd);
      }      
      else
      {
         newInfo.triangleIDs[1] = -1;
      }

      mEdgeInfoList.push_back(newInfo);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::ExtractErrorFromMesh(const dtAnim::PoseMesh &mesh)
{
   // Make sure no animations are applied
   mModel->ClearAll();
   mModel->Update(0.0f);

   const dtAnim::PoseMesh::TriangleVector &triangleList = mesh.GetTriangles();

   // Sample points along a grid for every triangle
   for (size_t triIndex = 0; triIndex < triangleList.size(); ++triIndex)
   {
      dtAnim::PoseMesh::Triangle tri = triangleList[triIndex]; 

      const osg::Vec3 &point1 = tri.mVertices[0]->mData * VERT_SCALE;
      const osg::Vec3 &point2 = tri.mVertices[1]->mData * VERT_SCALE;
      const osg::Vec3 &point3 = tri.mVertices[2]->mData * VERT_SCALE;

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
      mMeshUtil->ClearPoses(mPoseMesh, mModel, 0.0f);
   }
   
}

/////////////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::IsItemMovable()
{
   return flags() & QGraphicsItem::ItemIsMovable;
}

/////////////////////////////////////////////////////////////////////////////////////////
float PoseMeshItem::GetErrorSample(const QPointF &samplePoint)
{  
   QPointF meshSpaceTrueValue;
   meshSpaceTrueValue.rx() = samplePoint.x() / VERT_SCALE;
   meshSpaceTrueValue.ry() = samplePoint.y() / VERT_SCALE;

   dtAnim::PoseMesh::TargetTriangle blendTarget;
   mPoseMesh->GetTargetTriangleData(meshSpaceTrueValue.x(),
                                    meshSpaceTrueValue.y(),
                                    blendTarget);

   mMeshUtil->BlendPoses(mPoseMesh, mModel, blendTarget);

   // Apply the blended pose for this sample
   mModel->Update(0.0f);

   osg::Quat boneRotation = mModel->GetBoneAbsoluteRotation(mPoseMesh->GetBoneID());
   
   // calculate a vector transformed by the rotation data.
   osg::Vec3 transformed = boneRotation * mPoseMesh->GetNativeForwardDirection();   

   // calculate the local azimuth and elevation for the transformed vector
   float az, el;

   osg::Vec3 pelvisForward(0, -1, 0);
   dtAnim::GetCelestialCoordinates(transformed, pelvisForward, az, el);

   QPointF meshSpaceActualValue(az, el);

   QLineF trueDirection(QPointF(), meshSpaceTrueValue);
   QLineF actualDirection(QPointF(), meshSpaceActualValue);   

   return trueDirection.angle(actualDirection);
}

/////////////////////////////////////////////////////////////////////////////////////////
QColor PoseMeshItem::GetErrorColor(float degreesOfError)
{
   const float maxError = 10.0f;

   QColor errorColor;

   float percentError = degreesOfError / maxError;
   dtUtil::ClampMax(percentError, 1.0f);

   // In HSV color, red is at 0 and blue is a 240 degree rotation
   const float blue = 2.0f / 3.0f;   
   errorColor.setHsvF(blue - percentError * blue, 1.0f, 1.0f);

   return errorColor;
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshItem::GetTriangleBoundingRect(const dtAnim::PoseMesh::Triangle &tri, QRectF &outRect)
{
   float left   = FLT_MAX;
   float right  = -FLT_MAX;
   float bottom = FLT_MAX;
   float top    = -FLT_MAX;

   for (size_t vertIndex = 0; vertIndex < 3; ++vertIndex)
   {
      const osg::Vec3 &vertex = tri.mVertices[vertIndex]->mData;
      if (vertex.x() < left)  { left   = vertex.x(); }
      if (vertex.x() > right) { right  = vertex.x(); }
      if (vertex.y() > top)   { top    = vertex.y(); }
      if (vertex.y() < bottom){ bottom = vertex.y(); }
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

/////////////////////////////////////////////////////////////////////////////////////////
bool PoseMeshItem::GetIntersectionBoundingPoints(const QLineF &intersector, 
                                                 const QLineF lines[3],
                                                 QPointF &outLeftMost,
                                                 QPointF &outRightMost)
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
