#include <dtActors/fenceactor.h>

#include <dtCore/object.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/arrayactorproperty.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/mapxml.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <dtUtil/exception.h>

#include <osgDB/ReadFile>

namespace dtActors
{
   ////////////////////////////////////////////////////////////////////////////////
   // FENCE POST GEOM DATA
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   bool FencePostGeomData::Initialize()
   {
      if (!mParent) return false;

      FencePostGeomNode* fenceParent = dynamic_cast<FencePostGeomNode*>(mParent);
      if (!fenceParent) return false;

      // Create the post geometry object.
      mPost = new dtCore::Object();
      if (!mPost.valid()) return false;

      //osg::Group* myGroup = mParent->GetOSGNode()->asGroup();
      //if (!myGroup) return false;
      //myGroup->addChild(mPost->GetOSGNode());
      mParent->AddChild(mPost.get());

      // Create Geometry objects to store all the segment quads.
      mSegGeom = new osg::Geometry();
      if (!mSegGeom.valid()) return false;

      mSegGeode = new osg::Geode();
      if (!mSegGeode.valid()) return false;

      // Add the points geometry to the geode.
      mSegGeode->addDrawable(mSegGeom.get());

      // Add the Geode to the transformable node.
      mSegOrigin = new dtCore::Transformable();
      if (!mSegOrigin.valid()) return false;

      mParent->AddChild(mSegOrigin.get());

      mSegEnabled = false;

      // Create all of our buffer arrays.
      mSegVertexList = new osg::Vec3Array();
      if (!mSegVertexList.valid()) return false;
      mSegTextureList = new osg::Vec2Array();
      if (!mSegTextureList.valid()) return false;
      mSegNormalList = new osg::Vec3Array();
      if (!mSegNormalList.valid()) return false;

      // Apply the buffer arrays to the geometry.
      mSegGeom->setVertexArray(mSegVertexList.get());

      mSegGeom->setTexCoordArray(0, mSegTextureList.get());

      mSegGeom->setColorArray(fenceParent->mSegColorList.get());
      mSegGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

      mSegGeom->setNormalArray(mSegNormalList.get());
      //mSegGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
      mSegGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

      // Create a texture object to use for the segment.
      mSegTexture = new osg::Texture2D();
      if (!mSegTexture.valid()) return false;

      mSegTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);

      osg::StateSet* stateset = mSegGeom->getOrCreateStateSet();
      if (!stateset) return false;

      stateset->setTextureAttributeAndModes(0, mSegTexture.get(), osg::StateAttribute::ON);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool FencePostGeomData::Shutdown()
   {
      if (!mParent) return false;

      mParent->RemoveChild(mPost.get());
      mPost = NULL;

      SetSegmentEnabled(false);
      mSegOrigin = NULL;

      mSegGeode->removeDrawable(mSegGeom.get());
      mSegGeode = NULL;

      mSegGeom->setVertexArray(NULL);
      mSegGeom->setTexCoordArray(0, NULL);
      mSegGeom->setColorArray(NULL);
      mSegGeom->setNormalArray(NULL);
      mSegGeom->removePrimitiveSet(0, mSegGeom->getNumPrimitiveSets());
      mSegGeom = NULL;

      mSegVertexList = NULL;
      mSegTextureList = NULL;
      mSegNormalList = NULL;

      mSegTexture = NULL;

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FencePostGeomData::SetSegmentEnabled(bool enabled)
   {
      if (enabled != mSegEnabled)
      {
         if (enabled)
         {
            osg::Group* originGroup = mSegOrigin->GetOSGNode()->asGroup();
            if (originGroup)
            {
               originGroup->addChild(mSegGeode.get());
            }
         }
         else
         {
            osg::Group* originGroup = mSegOrigin->GetOSGNode()->asGroup();
            if (originGroup)
            {
               originGroup->removeChild(mSegGeode.get());
            }
         }

         mSegEnabled = enabled;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FencePostGeomData::SetQuadCount(int count)
   {
      // Ignore if the size is not changed.
      if (count < 0 || count == (int)mSegNormalList->size())
      {
         return;
      }

      mSegVertexList->resize(count * 4);
      mSegTextureList->resize(count * 4);
      mSegNormalList->resize(count);

      // Remove all the current primitives.
      mSegGeom->removePrimitiveSet(0, mSegGeom->getNumPrimitiveSets());

      // Now Set the primitive sets for the three sides of the segment.
      for (int index = 0; index < count; index++)
      {
         mSegGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, index * 4, 4));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // FENCE POST GEOM NODE
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   FencePostGeomNode::FencePostGeomNode()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   FencePostGeomNode::~FencePostGeomNode()
   {
      SetSize(0);
      mSegColorList = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   LinkedPointsGeomDataBase* FencePostGeomNode::CreateGeom()
   {
      // Make sure our color list array exists.
      if (!mSegColorList.valid())
      {
         mSegColorList = new osg::Vec4Array();
         mSegColorList->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      }

      return new FencePostGeomData(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   FenceActor::FenceActor(dtActors::LinkedPointsActorProxy* proxy, const std::string& name)
      : BaseClass(proxy, name)
      , mPostMinDistance(1.0f)
      , mPostMaxDistance(10.0f)
      , mFenceScale(1.0f)
      , mSegmentHeight(5.0f)
      , mSegmentWidth(1.0f)
      , mTopTextureRatio(0.2f)
   {
      mSegmentPointList.push_back(SegmentPointData(osg::Vec2(0.0f, 1.0f)));
      mSegmentPointList.push_back(SegmentPointData());
   }

   /////////////////////////////////////////////////////////////////////////////
   FenceActor::~FenceActor()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::Visualize()
   {
      BaseClass::Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::Visualize(int pointIndex)
   {
      if (mProxy->IsLoading())
      {
         return;
      }

      // If there are no meshes setup for the posts yet, use the default sphere/line schema.
      if (mPostResourceList.empty() || mSegmentPointList.empty())
      {
         BaseClass::Visualize(pointIndex);
         return;
      }

      FencePostGeomNode* point = dynamic_cast<FencePostGeomNode*>(GetPointDrawable(pointIndex));
      // If the drawable is not the proper class, regenerate it.
      if (!point)
      {
         RegeneratePointDrawable(pointIndex);

         point = dynamic_cast<FencePostGeomNode*>(GetPointDrawable(pointIndex));
      }

      if (point)
      {
         // Always place a post directly on the point.
         osg::Vec3 postPos = GetPointPosition(pointIndex);
         dtCore::Transform prevPostTransform;
         dtCore::Transform postTransform;

         // Now generate the fence posts and segments up to the next point position.
         if (pointIndex < GetPointCount() - 1)
         {
            prevPostTransform = PlacePost(pointIndex, 0, postPos);

            int nextPoint = pointIndex + 1;

            // Get our start and end points.
            osg::Vec3 start = GetPointPosition(pointIndex);
            osg::Vec3 end = GetPointPosition(nextPoint);

            osg::Vec3 dir = end - start;
            float totalLength = dir.length();
            dir.normalize();

            // Now determine the position of each post.
            osg::Vec3 lastPost = start;
            int subPostIndex = 1;
            float length = 0.0f;

            int startPostIndex = 0;
            std::string startTextureName = GetSegmentTexture(pointIndex, subPostIndex - 1);
            dtCore::Transform startPostTransform = prevPostTransform;

            while(true)
            {
               if (!point->SetIndex(subPostIndex - 1))
               {
                  return;
               }

               FencePostGeomData* geomData = dynamic_cast<FencePostGeomData*>(point->mGeomList[subPostIndex - 1].get());
               if (!geomData) return;

               geomData->SetSegmentEnabled(false);

               std::string textureName = GetSegmentTexture(pointIndex, subPostIndex);

               osg::Vec3 position = lastPost + (dir * mPostMaxDistance);
               length += mPostMaxDistance;

               // Bail out if we have reached the end of our length.
               if (length >= totalLength - mPostMinDistance)
               {
                  // Make sure we use the end point as the end transform for the segment.
                  dtCore::Transformable* endPoint = GetPointDrawable(nextPoint);
                  assert(endPoint);
                  endPoint->GetTransform(postTransform);

                  // Calculate the texture length of the segment.
                  float len = (postTransform.GetTranslation() - prevPostTransform.GetTranslation()).length();
                  float texLength = len / mPostMaxDistance;

                  // Now place a segment to fill the gap between our last post and the next point.
                  PlaceSegment(pointIndex, startPostIndex, startPostTransform, postTransform, texLength + (subPostIndex - startPostIndex - 1));

                  // Now make sure we clean up any additional indices.
                  point->SetSize(subPostIndex);
                  break;
               }

               // Place a post if we are not too close to the end.
               if (totalLength - length > mPostMinDistance)
               {
                  postTransform = PlacePost(pointIndex, subPostIndex, position);
               }

               // Now place a segment between the current post and the previous.
               if (startTextureName != textureName)
               {
                  PlaceSegment(pointIndex, startPostIndex, startPostTransform, postTransform, float(subPostIndex - startPostIndex));
                  startPostIndex = subPostIndex;
                  startPostTransform = postTransform;
                  startTextureName = textureName;
               }
               subPostIndex++;

               lastPost = position;
               prevPostTransform = postTransform;
            }
         }
         // Make sure we clear any geometry on the last post.
         else
         {
            point->SetSize(1);

            prevPostTransform = PlacePost(pointIndex, 0, postPos);
            PlaceSegment(pointIndex, 0, prevPostTransform, prevPostTransform, 1.0f);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int FenceActor::GetPointIndex(dtCore::DeltaDrawable* drawable)
   {
      return BaseClass::GetPointIndex(drawable);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int FenceActor::GetPointIndex(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPos)
   {
      int pointIndex = BaseClass::GetPointIndex(drawable);

      if (pointIndex >= 0 && pointIndex + 1 < GetPointCount())
      {
         osg::Vec3 firstPos = GetPointPosition(pointIndex);
         osg::Vec3 secondPos = GetPointPosition(pointIndex + 1);
         float fDistance1 = (pickPos - firstPos).length();
         float fDistance2 = (pickPos - secondPos).length();

         // If we are closer to our second point, then change our selection index.
         if (fDistance1 > fDistance2)
         {
            pointIndex++;
         }
      }

      return pointIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   FenceActor::DrawableType FenceActor::GetDrawableType(dtCore::DeltaDrawable* drawable, osg::Vec3 position, int& pointIndex, int& subIndex)
   {
      pointIndex = BaseClass::GetPointIndex(drawable);

      if (pointIndex >= 0)
      {
         FencePostGeomNode* geom = dynamic_cast<FencePostGeomNode*>(GetPointDrawable(pointIndex));
         if (geom)
         {
            for (subIndex = 0; subIndex < (int)geom->mGeomList.size(); subIndex++)
            {
               FencePostGeomData* data = dynamic_cast<FencePostGeomData*>(geom->mGeomList[subIndex].get());
               if (data)
               {
                  if (drawable == data->mPost.get())
                  {
                     return DRAWABLE_TYPE_POST;
                  }
                  else if (drawable == data->mSegOrigin.get())
                  {
                     if (mPostMaxDistance > 0)
                     {
                        // Now figure out which segment was clicked on.
                        osg::Vec3 start = GetPointPosition(pointIndex);
                        osg::Vec3 end = GetPointPosition(pointIndex + 1);

                        osg::Vec3 closestPoint = FindNearestPointOnLine(start, end, position);
                        float posLen = (closestPoint - start).length();
                        float fullLen = (start - end).length();

                        float index = posLen / mPostMaxDistance;
                        subIndex = int(floor(index));

                        float len = fullLen - (subIndex * mPostMaxDistance);
                        if (len <= mPostMinDistance)
                        {
                           subIndex--;
                        }
                     }

                     return DRAWABLE_TYPE_SEGMENT;
                  }
               }
            }
         }
      }

      return DRAWABLE_TYPE_NONE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::ResourceDescriptor> FenceActor::GetPostResourceArray(void) const
   {
      return mPostResourceList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetPostResourceArray(const std::vector<dtCore::ResourceDescriptor>& value)
   {
      mPostResourceList = value;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::ResourceDescriptor> FenceActor::GetSegmentResourceArray(void) const
   {
      return mSegmentResourceList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetSegmentResourceArray(const std::vector<dtCore::ResourceDescriptor>& value)
   {
      mSegmentResourceList = value;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<FenceActor::ResourceIDData> FenceActor::GetResourceIDArray()
   {
      return mResourceIDList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetResourceIDArray(const std::vector<FenceActor::ResourceIDData>& value)
   {
      mResourceIDList = value;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<FenceActor::SegmentPointData> FenceActor::GetSegmentPointArray()
   {
      return mSegmentPointList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetSegmentPointArray(const std::vector<SegmentPointData>& value)
   {
      mSegmentPointList = value;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string FenceActor::GetPostMesh(int pointIndex, int subIndex)
   {
      int meshIndex = 0;

      // Iterate through the resource ID list for a matching post/segment.
      for (int index = 0; index < (int)mResourceIDList.size(); index++)
      {
         if (mResourceIDList[index].pointIndex == pointIndex &&
            mResourceIDList[index].segmentIndex == subIndex)
         {
            meshIndex = mResourceIDList[index].postID;
         }
      }

      // Now find the mesh matching this index.
      if (meshIndex < (int)mPostResourceList.size())
      {
         dtCore::ResourceDescriptor& rd = mPostResourceList[meshIndex];
         if (rd != dtCore::ResourceDescriptor::NULL_RESOURCE)
         {
            return dtCore::Project::GetInstance().GetResourcePath(rd);
         }
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetPostMesh(int pointIndex, int subIndex, int meshIndex)
   {
      // Iterate through the resource ID list for a matching post/segment.
      for (int index = 0; index < (int)mResourceIDList.size(); index++)
      {
         if (mResourceIDList[index].pointIndex == pointIndex &&
            mResourceIDList[index].segmentIndex == subIndex)
         {
            // If we already have an entry, update it and finish.
            mResourceIDList[index].postID = meshIndex;
            return;
         }
      }

      // If we get here, it means we don't already have an entry, so create one.
      FenceActor::ResourceIDData entry;
      entry.pointIndex = pointIndex;
      entry.segmentIndex = subIndex;
      entry.postID = meshIndex;
      entry.segmentID = 0;
      mResourceIDList.push_back(entry);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::IncrementPostMesh(int pointIndex, int subIndex)
   {
      if (mPostResourceList.size() <= 1)
      {
         return;
      }

      // Iterate through the resource ID list for a matching post/segment.
      for (int index = 0; index < (int)mResourceIDList.size(); index++)
      {
         if (mResourceIDList[index].pointIndex == pointIndex &&
            mResourceIDList[index].segmentIndex == subIndex)
         {
            // If we already have an entry, update it and finish.
            int meshIndex = mResourceIDList[index].postID + 1;
            if (meshIndex >= (int)mPostResourceList.size()) meshIndex = 0;
            mResourceIDList[index].postID = meshIndex;
            Visualize(pointIndex);
            return;
         }
      }

      // If we get here, it means we don't already have an entry, so create one.
      if (mPostResourceList.size() > 0)
      {
         FenceActor::ResourceIDData entry;
         entry.pointIndex = pointIndex;
         entry.segmentIndex = subIndex;
         entry.postID = 1;
         entry.segmentID = 0;
         mResourceIDList.push_back(entry);
         Visualize(pointIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetPostMinDistance(float value)
   {
      mPostMinDistance = value;

      // Clamp the size.
      if (mPostMinDistance <= 0.0f) mPostMinDistance = 0.0f;
      if (mPostMinDistance > mPostMaxDistance) mPostMinDistance = mPostMaxDistance;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetPostMaxDistance(float value)
   {
      mPostMaxDistance = value;

      // Clamp the size.
      if (mPostMaxDistance < 0.0f) mPostMaxDistance = 0.0f;
      if (mPostMaxDistance < mPostMinDistance) mPostMaxDistance = mPostMinDistance;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetFenceScale(float value)
   {
      mFenceScale = value;

      if (mFenceScale < 0.01f) mFenceScale = 0.01f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string FenceActor::GetSegmentTexture(int pointIndex, int subIndex)
   {
      int textureIndex = 0;

      // Iterate through the resource ID list for a matching post/segment.
      for (int index = 0; index < (int)mResourceIDList.size(); index++)
      {
         if (mResourceIDList[index].pointIndex == pointIndex &&
            mResourceIDList[index].segmentIndex == subIndex)
         {
            textureIndex = mResourceIDList[index].segmentID;
         }
      }

      // Now find the mesh matching this index.
      if (textureIndex < (int)mSegmentResourceList.size())
      {
         dtCore::ResourceDescriptor& rd = mSegmentResourceList[textureIndex];
         if (rd != dtCore::ResourceDescriptor::NULL_RESOURCE)
         {
            return dtCore::Project::GetInstance().GetResourcePath(rd);
         }
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetSegmentTexture(int pointIndex, int subIndex, int textureIndex)
   {
      // Iterate through the resource ID list for a matching post/segment.
      for (int index = 0; index < (int)mResourceIDList.size(); index++)
      {
         if (mResourceIDList[index].pointIndex == pointIndex &&
            mResourceIDList[index].segmentIndex == subIndex)
         {
            // If we already have an entry, update it and finish.
            mResourceIDList[index].segmentID = textureIndex;
            return;
         }
      }

      // If we get here, it means we don't already have an entry, so create one.
      FenceActor::ResourceIDData entry;
      entry.pointIndex = pointIndex;
      entry.segmentIndex = subIndex;
      entry.postID = 0;
      entry.segmentID = textureIndex;
      mResourceIDList.push_back(entry);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::IncrementSegmentMesh(int pointIndex, int subIndex)
   {
      if (mSegmentResourceList.size() <= 1)
      {
         return;
      }

      // Iterate through the resource ID list for a matching post/segment.
      for (int index = 0; index < (int)mResourceIDList.size(); index++)
      {
         if (mResourceIDList[index].pointIndex == pointIndex &&
            mResourceIDList[index].segmentIndex == subIndex)
         {
            // If we already have an entry, update it and finish.
            int textureIndex = mResourceIDList[index].segmentID + 1;
            if (textureIndex >= (int)mSegmentResourceList.size()) textureIndex = 0;
            mResourceIDList[index].segmentID = textureIndex;
            Visualize(pointIndex);
            return;
         }
      }

      // If we get here, it means we don't already have an entry, so create one.
      if (mSegmentResourceList.size() > 0)
      {
         FenceActor::ResourceIDData entry;
         entry.pointIndex = pointIndex;
         entry.segmentIndex = subIndex;
         entry.postID = 0;
         entry.segmentID = 1;
         mResourceIDList.push_back(entry);
         Visualize(pointIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetSegmentHeight(float value)
   {
      mSegmentHeight = value;

      if (mSegmentHeight < 0.0f) mSegmentHeight = 0.0f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetSegmentWidth(float value)
   {
      mSegmentWidth = value;

      if (mSegmentWidth < 0.0f) mSegmentWidth = 0.0f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetTopTextureRatio(float value)
   {
      mTopTextureRatio = value;

      if (mTopTextureRatio < 0.0f) mTopTextureRatio = 0.0f;
      if (mTopTextureRatio > 1.0f) mTopTextureRatio = 1.0f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FenceActor::GetScale() const
   {
      return osg::Vec3(mPostMaxDistance, 1.0f, mFenceScale);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetScale(const osg::Vec3& value)
   {
      mPostMaxDistance = value.x();
      if (mPostMaxDistance <= 1.0f) mPostMaxDistance = 1.0f;
      if (mPostMaxDistance < mPostMinDistance) mPostMaxDistance = mPostMinDistance;

      //mSegmentWidth = value.y();
      //if (mSegmentWidth < 0.0f) mSegmentWidth = 0.0f;

      mFenceScale = value.z();

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* FenceActor::CreatePointDrawable(osg::Vec3 position)
   {
      // If we are not ready to render our fence, default to the parent rendering.
      if (mPostResourceList.empty() || mSegmentPointList.empty())
      {
         return BaseClass::CreatePointDrawable(position);
      }

      dtCore::Transformable* point = new FencePostGeomNode();

      // Attach this new point to the actor.
      AddChild(point);

      // Set the position of this point.
      dtCore::Transform transform;
      point->GetTransform(transform);
      transform.SetTranslation(position);
      point->SetTransform(transform);


      return point;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Transform FenceActor::PlacePost(int pointIndex, int subIndex, osg::Vec3 position)
   {
      dtCore::Transform transform;
      transform.SetTranslation(position);

      dtCore::Transformable* point = GetPointDrawable(pointIndex);
      if (point)
      {
         FencePostGeomNode* fencePost = dynamic_cast<FencePostGeomNode*>(point);
         if (fencePost)
         {
            std::string postMeshName = GetPostMesh(pointIndex, subIndex);
            if (!postMeshName.empty())
            {
               // Create our game mesh actor to represent the post.
               if (!fencePost->SetIndex(subIndex))
               {
                  return transform;
               }

               FencePostGeomData* geomData = dynamic_cast<FencePostGeomData*>(fencePost->mGeomList[subIndex].get());
               if (!geomData) return transform;

               dtCore::RefPtr<dtCore::Object> postMesh = geomData->mPost;

               postMesh->SetScale(osg::Vec3(mFenceScale, mFenceScale, mFenceScale));

               if (postMesh->GetFilename() != postMeshName)
               {
                  postMesh->LoadFile(postMeshName);
               }

               // Set the position of the object.
               postMesh->GetTransform(transform);
               transform.SetTranslation(position);
               postMesh->SetTransform(transform);

               // Place collision geometry between primary posts.
               if (pointIndex > 0 && subIndex == 0)
               {
                  // Make sure we have collision geometry for this node.
                  if (!geomData->mCollision.valid())
                  {
                     geomData->mCollision = new dtCore::Transformable();
                     postMesh->AddChild(geomData->mCollision);
                  }

                  // Set up collision geometry.
                  FencePostGeomNode* prevFencePost = dynamic_cast<FencePostGeomNode*>(GetPointDrawable(pointIndex - 1));
                  if (prevFencePost && prevFencePost->mGeomList.size())
                  {
                     FencePostGeomData* prevGeomData = dynamic_cast<FencePostGeomData*>(prevFencePost->mGeomList[0].get());
                     if (prevGeomData && prevGeomData->mPost.valid())
                     {
                        dtCore::Transform prevTransform;
                        prevGeomData->mPost->GetTransform(prevTransform);

                        osg::Vec3 pos = transform.GetTranslation();
                        osg::Vec3 prevPos = prevTransform.GetTranslation();

                        // Position the collision transform in the midpoint between
                        // the posts.
                        osg::Vec3 vec = prevPos - pos;
                        osg::Vec3 mid = (vec * 0.5f) + pos;

                        // Calculate the size of the collision box.
                        float xMin = 0.0f;
                        float xMax = 0.0f;
                        float yMin = 0.0f;
                        float yMax = 0.0f;
                        for (int index = 0; index < (int)mSegmentPointList.size(); index++)
                        {
                           SegmentPointData& data = mSegmentPointList[index];

                           if (index != 0)
                           {
                              if (data.position.x() < xMin) xMin = data.position.x();
                              if (data.position.x() > xMax) xMax = data.position.x();
                              if (data.position.y() < yMin) yMin = data.position.y();
                              if (data.position.y() > yMax) yMax = data.position.y();
                           }
                           else
                           {
                              xMin = xMax = data.position.x();
                              yMin = yMax = data.position.y();
                           }
                        }

                        //float width = ((xMax - xMin) + mSegmentWidth) * mSegmentHeight * mFenceScale * 2.0f;
                        float height = (yMax - yMin) * mSegmentHeight * mFenceScale;
                        //float length = vec.length();

                        // Offset the height of the collision center based on the geometry.
                        osg::Vec3 offset;
                        offset.z() = (yMin * mSegmentHeight * mFenceScale) + (height * 0.5f);

                        dtCore::Transform colTransform;
                        colTransform.Set(mid + offset, prevPos + offset, osg::Vec3(0, 0, 1));
                     }
                  }
               }
            }
         }
      }

      return transform;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::PlaceSegment(int pointIndex, int subIndex, dtCore::Transform start, dtCore::Transform end, float length)
   {
      dtCore::Transformable* point = GetPointDrawable(pointIndex);
      if (point)
      {
         FencePostGeomNode* fencePost = dynamic_cast<FencePostGeomNode*>(point);
         if (fencePost)
         {
            if (!fencePost->SetIndex(subIndex))
            {
               return;
            }

            FencePostGeomData* geomData = dynamic_cast<FencePostGeomData*>(fencePost->mGeomList[subIndex].get());
            if (!geomData) return;

            geomData->SetSegmentEnabled(true);

            // Always reset the origin transform to the origin of the world.
            dtCore::Transform segmentTransform;
            geomData->mSegOrigin->SetTransform(segmentTransform);

            osg::Vec3 startRight, startUp, startForward;
            osg::Vec3 endRight, endUp, endForward;

            // Get the orientation of our two posts.
            start.GetOrientation(startRight, startUp, startForward);
            end.GetOrientation(endRight, endUp, endForward);

            // Get the vector between the two posts.
            osg::Vec3 dir = end.GetTranslation() - start.GetTranslation();
            dir.normalize();

            // Now get the vectors that will be used to calculate segment width
            osg::Vec3 startWidthN = startUp ^ dir;
            startWidthN.normalize();
            osg::Vec3 startWidth = startWidthN * mSegmentWidth * 0.5f * mFenceScale;

            osg::Vec3 endWidthN = endUp ^ dir;
            endWidthN.normalize();
            osg::Vec3 endWidth = endWidthN * mSegmentWidth * 0.5f * mFenceScale;

            int quadCount = ((mSegmentPointList.size() - 1) * 2) + 1;
            if (subIndex == 0)
            {
               if (pointIndex > 0 && pointIndex < (int)GetPointList().size() - 1)
               {
                  geomData->SetQuadCount(quadCount * 2);
               }
               else
               {
                  geomData->SetQuadCount(quadCount + (quadCount / 2));
               }
            }
            else
            {
               geomData->SetQuadCount(quadCount);
            }

            // Top
            {
               PlaceSegmentQuad(geomData, 0,
                  (start.GetTranslation() - (startWidth + (startWidthN * mSegmentPointList[0].position.x() * mSegmentHeight * mFenceScale))) + (startUp * mSegmentHeight * mFenceScale * mSegmentPointList[0].position.y()),
                  (start.GetTranslation() + (startWidth + (startWidthN * mSegmentPointList[0].position.x() * mSegmentHeight * mFenceScale))) + (startUp * mSegmentHeight * mFenceScale * mSegmentPointList[0].position.y()),
                  (end.GetTranslation() - (endWidth + (endWidthN * mSegmentPointList[0].position.x() * mSegmentHeight * mFenceScale))) + (endUp * mSegmentHeight * mFenceScale * mSegmentPointList[0].position.y()),
                  (end.GetTranslation() + (endWidth + (endWidthN * mSegmentPointList[0].position.x() * mSegmentHeight * mFenceScale))) + (endUp * mSegmentHeight * mFenceScale * mSegmentPointList[0].position.y()),
                  osg::Vec2(0.0f, 1.0f), osg::Vec2(length, 1.0f), osg::Vec2(0.0f, 1.0f - mTopTextureRatio), osg::Vec2(length, 1.0f - mTopTextureRatio));
            }

            // Sides
            for (int index = 0; index < (int)mSegmentPointList.size() - 1; index++)
            {
               float texTop = 0.0f;
               float texBot = 0.0f;

               if(mSegmentPointList[index].textureHeight < 0.0f)
                  texTop = (1.0f - mTopTextureRatio) * mSegmentPointList[index].position.y();
               else
                  texTop = (1.0f - mTopTextureRatio) * mSegmentPointList[index].textureHeight;

               if(mSegmentPointList[index + 1].textureHeight < 0.0f)
                  texBot = (1.0f - mTopTextureRatio) * mSegmentPointList[index + 1].position.y();
               else
                  texBot = (1.0f - mTopTextureRatio) * mSegmentPointList[index + 1].textureHeight;

               int quadIndex = (index * 2) + 1;
               PlaceSegmentQuad(geomData, quadIndex,
                  (start.GetTranslation() + (startWidth + (startWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.x()))) + (startUp * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.y()),
                  (start.GetTranslation() + (startWidth + (startWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.x()))) + (startUp * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.y()),
                  (end.GetTranslation() + (endWidth + (endWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.x()))) + (endUp * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.y()),
                  (end.GetTranslation() + (endWidth + (endWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.x()))) + (endUp * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.y()),
                  osg::Vec2(0.0f, texTop), osg::Vec2(length, texTop), osg::Vec2(0.0f, texBot), osg::Vec2(length, texBot));
               PlaceSegmentQuad(geomData, quadIndex + 1,
                  (start.GetTranslation() - (startWidth + (startWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.x()))) + (startUp * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.y()),
                  (start.GetTranslation() - (startWidth + (startWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.x()))) + (startUp * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.y()),
                  (end.GetTranslation() - (endWidth + (endWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.x()))) + (endUp * mSegmentHeight * mFenceScale * mSegmentPointList[index + 1].position.y()),
                  (end.GetTranslation() - (endWidth + (endWidthN * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.x()))) + (endUp * mSegmentHeight * mFenceScale * mSegmentPointList[index].position.y()),
                  osg::Vec2(0.0f, texBot), osg::Vec2(length, texBot), osg::Vec2(0.0f, texTop), osg::Vec2(length, texTop));
            }

            // Now connect the geometry of this segment to the previous
            if (subIndex == 0)
            {
               if (pointIndex > 0 && pointIndex < (int)GetPointList().size() - 1)
               {
                  FencePostGeomNode* prevPost = dynamic_cast<FencePostGeomNode*>(GetPointDrawable(pointIndex - 1));
                  if (prevPost)
                  {
                     FencePostGeomData* prevGeomData = NULL;

                     // Find the last enabled segment.
                     for (int index = 0; index < (int)prevPost->mGeomList.size(); index++)
                     {
                        FencePostGeomData* testGeomData = dynamic_cast<FencePostGeomData*>(prevPost->mGeomList[index].get());
                        if (testGeomData && testGeomData->mSegEnabled)
                        {
                           prevGeomData = testGeomData;
                        }
                     }

                     if (!prevGeomData) return;

                     // Make sure the previous post has the proper geometry.
                     if ((int)prevGeomData->mSegNormalList->size() >= quadCount)
                     {
                        for (int index = 0; index < quadCount; index++)
                        {
                           // Calculate the texture coordinates.
                           float texTop = geomData->mSegTextureList->at((index * 4) + 2).y();
                           float texBot = geomData->mSegTextureList->at((index * 4) + 3).y();

                           osg::Vec2 texTopLeft  = osg::Vec2(0.0f, texTop);
                           osg::Vec2 texTopRight = osg::Vec2((prevGeomData->mSegVertexList->at((index * 4) + 2) - geomData->mSegVertexList->at((index * 4) + 1)).length() / mPostMaxDistance, texTop);
                           osg::Vec2 texBotLeft  = osg::Vec2(0.0f, texBot);
                           osg::Vec2 texBotRight = osg::Vec2((prevGeomData->mSegVertexList->at((index * 4) + 3) - geomData->mSegVertexList->at((index * 4) + 0)).length() / mPostMaxDistance, texBot);

                           PlaceSegmentQuad(geomData, quadCount + index,
                              prevGeomData->mSegVertexList->at((index * 4) + 2),
                              prevGeomData->mSegVertexList->at((index * 4) + 3),
                              geomData->mSegVertexList->at((index * 4) + 1),
                              geomData->mSegVertexList->at((index * 4) + 0),
                              texTopLeft, texTopRight, texBotLeft, texBotRight);
                        }
                     }
                  }
               }
               // If this is an end post, then cap the end.
               else if (pointIndex == 0)
               {
                  if ((int)geomData->mSegNormalList->size() >= quadCount)
                  {
                     int halfCount = quadCount / 2 + 1;
                     for (int index = 1; index < halfCount; index++)
                     {
                        float texTop = geomData->mSegTextureList->at((index * 8) + 1).y();
                        float texBot = geomData->mSegTextureList->at((index * 8) + 0).y();

                        osg::Vec2 texTopLeft  = osg::Vec2(0.0f, texTop);
                        osg::Vec2 texTopRight = osg::Vec2((geomData->mSegVertexList->at((index * 8) - 4) - geomData->mSegVertexList->at((index * 8) + 1)).length() / mPostMaxDistance, texTop);
                        osg::Vec2 texBotLeft  = osg::Vec2(0.0f, texBot);
                        osg::Vec2 texBotRight = osg::Vec2((geomData->mSegVertexList->at((index * 8) - 3) - geomData->mSegVertexList->at((index * 8) + 0)).length() / mPostMaxDistance, texBot);

                        PlaceSegmentQuad(geomData, quadCount + index - 1,
                           geomData->mSegVertexList->at((index * 8) - 4),
                           geomData->mSegVertexList->at((index * 8) - 3),
                           geomData->mSegVertexList->at((index * 8) + 1),
                           geomData->mSegVertexList->at((index * 8) + 0),
                           texTopLeft, texTopRight, texBotLeft, texBotRight);
                     }
                  }
               }
               else
               {
                  FencePostGeomNode* prevPost = dynamic_cast<FencePostGeomNode*>(GetPointDrawable(pointIndex - 1));
                  if (prevPost)
                  {
                     FencePostGeomData* prevGeomData = NULL;

                     // Find the last enabled segment.
                     for (int index = 0; index < (int)prevPost->mGeomList.size(); index++)
                     {
                        FencePostGeomData* testGeomData = dynamic_cast<FencePostGeomData*>(prevPost->mGeomList[index].get());
                        if (testGeomData && testGeomData->mSegEnabled)
                        {
                           prevGeomData = testGeomData;
                        }
                     }

                     if (!prevGeomData) return;

                     if ((int)prevGeomData->mSegNormalList->size() >= quadCount)
                     {
                        int halfCount = quadCount / 2 + 1;
                        for (int index = 1; index < halfCount; index++)
                        {
                           float texTop = prevGeomData->mSegTextureList->at((index * 8) + 2).y();
                           float texBot = prevGeomData->mSegTextureList->at((index * 8) + 3).y();

                           osg::Vec2 texTopLeft  = osg::Vec2(0.0f, texTop);
                           osg::Vec2 texTopRight = osg::Vec2((prevGeomData->mSegVertexList->at((index * 8) + 2) - prevGeomData->mSegVertexList->at((index * 8) - 1)).length() / mPostMaxDistance, texTop);
                           osg::Vec2 texBotLeft  = osg::Vec2(0.0f, texBot);
                           osg::Vec2 texBotRight = osg::Vec2((prevGeomData->mSegVertexList->at((index * 8) + 3) - prevGeomData->mSegVertexList->at((index * 8) - 2)).length() / mPostMaxDistance, texBot);

                           PlaceSegmentQuad(geomData, quadCount + index - 1,
                              prevGeomData->mSegVertexList->at((index * 8) + 2),
                              prevGeomData->mSegVertexList->at((index * 8) + 3),
                              prevGeomData->mSegVertexList->at((index * 8) - 1),
                              prevGeomData->mSegVertexList->at((index * 8) - 2),
                              texTopLeft, texTopRight, texBotLeft, texBotRight);
                        }
                     }
                  }
               }
            }

            // Get the texture to use for this segment.
            std::string textureName = GetSegmentTexture(pointIndex, subIndex);
            if (!textureName.empty())
            {
               // set up the texture state.
               //dtCore::ResourceDescriptor descriptor = dtCore::ResourceDescriptor(textureName);
               //geomData->mSegTexture->setImage(osgDB::readImageFile(dtCore::Project::GetInstance().GetResourcePath(descriptor)));
               geomData->mSegTexture->setImage(osgDB::readImageFile(textureName));
            }

            // Make sure the geometry node knows to re-calculate the bounding area and display lists.
            geomData->mSegGeom->dirtyBound();
            geomData->mSegGeom->dirtyDisplayList();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::PlaceSegmentQuad(FencePostGeomData* geomData, int quadIndex, osg::Vec3 startTop, osg::Vec3 startBot, osg::Vec3 endTop, osg::Vec3 endBot, osg::Vec2 texTopLeft, osg::Vec2 texTopRight, osg::Vec2 texBotLeft, osg::Vec2 texBotRight)
   {
      geomData->mSegVertexList->at((quadIndex * 4) + 0) = startBot;
      geomData->mSegVertexList->at((quadIndex * 4) + 1) = startTop;
      geomData->mSegVertexList->at((quadIndex * 4) + 2) = endTop;
      geomData->mSegVertexList->at((quadIndex * 4) + 3) = endBot;

      geomData->mSegTextureList->at((quadIndex * 4) + 0) = texBotLeft; //(osg::Vec2(texStart, texBot));
      geomData->mSegTextureList->at((quadIndex * 4) + 1) = texTopLeft; //(osg::Vec2(texStart, texTop));
      geomData->mSegTextureList->at((quadIndex * 4) + 2) = texTopRight;//(osg::Vec2(texEnd, texTop));
      geomData->mSegTextureList->at((quadIndex * 4) + 3) = texBotRight;//(osg::Vec2(texEnd, texBot));

      // Calculate the surface normal.
      osg::Vec3 bottomToTopVec = startTop - startBot;
      bottomToTopVec.normalize();

      osg::Vec3 startToEndVec = endBot - startBot;
      startToEndVec.normalize();

      osg::Vec3 normal = bottomToTopVec ^ startToEndVec;
      normal.normalize();

      geomData->mSegNormalList->at(quadIndex) = normal;
   }

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   FenceActorProxy::FenceActorProxy()
      : BaseClass()
      , mPostResourceIndex(0)
      , mSegmentResourceIndex(0)
      , mResourceIDIndex(0)
   {
      SetClassName("dtActors::FenceActor");
   }

   /////////////////////////////////////////////////////////////////////////////
   FenceActorProxy::~FenceActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::CreateDrawable()
   {
      LinkedPointsActor* actor = new FenceActor(this);
      SetDrawable(*actor);
      actor->Initialize();
   }

   /////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      FenceActor* actor = NULL;
      GetDrawable(actor);

      // Post Resources Array
      dtCore::ResourceActorProperty* postResourceProp = new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
         "PostMeshResource", "Post Mesh",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &FenceActorProxy::SetPostMesh),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &FenceActorProxy::GetPostMesh),
         "Defines the mesh used to represent a fence post.", "Fence");

      dtCore::ArrayActorPropertyBase* postResourceArrayProp =
         new dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>(
         "PostMeshArray", "Post Meshes",
         "Defines the meshes used to represent the fence posts.",
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::SetIndexFuncType(this, &FenceActorProxy::SetPostResourceIndex),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::GetDefaultFuncType(this, &FenceActorProxy::GetDefaultPost),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::GetArrayFuncType(actor, &FenceActor::GetPostResourceArray),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::SetArrayFuncType(actor, &FenceActor::SetPostResourceArray),
         postResourceProp, "Fence");
      AddProperty(postResourceArrayProp);

      // Segment Resources Array
      dtCore::ResourceActorProperty* segmentResourceProp =
         new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "SegmentTextureResource", "Segment Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &FenceActorProxy::SetSegmentTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &FenceActorProxy::GetSegmentTexture),
         "Defines the texture used to represent a fence segment.", "Fence");

      dtCore::ArrayActorPropertyBase* segmentResourceArrayProp =
         new dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>(
         "SegmentTextureArray", "Segment Textures",
         "Defines the textures used to represent the fence segments.",
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::SetIndexFuncType(this, &FenceActorProxy::SetSegmentResourceIndex),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::GetDefaultFuncType(this, &FenceActorProxy::GetDefaultSegment),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::GetArrayFuncType(actor, &FenceActor::GetSegmentResourceArray),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::SetArrayFuncType(actor, &FenceActor::SetSegmentResourceArray),
         segmentResourceProp, "Fence");
      AddProperty(segmentResourceArrayProp);

      // Segment Points.
      dtCore::Vec2ActorProperty* segmentPointProp =
         new dtCore::Vec2ActorProperty(
         "SegmentPoint", "Position",
         dtCore::Vec2ActorProperty::SetFuncType(this, &FenceActorProxy::SetSegmentPoint),
         dtCore::Vec2ActorProperty::GetFuncType(this, &FenceActorProxy::GetSegmentPoint),
         "Stores the point data for a segment.",
         "Fence");

      dtCore::FloatActorProperty* segmentTexProp =
         new dtCore::FloatActorProperty(
         "TextureY", "Texture Y",
         dtCore::FloatActorProperty::SetFuncType(this, &FenceActorProxy::SetSegmentPointTextureHeight),
         dtCore::FloatActorProperty::GetFuncType(this, &FenceActorProxy::GetSegmentPointTextureHeight),
         "The vertical texture coordinate.",
         "Fence");

      dtCore::ContainerActorProperty* segmentContainerProp =
         new dtCore::ContainerActorProperty(
         "SegmentPointData", "Segment Point",
         "The segment geometry point data.",
         "Fence");
      segmentContainerProp->AddProperty(segmentPointProp);
      segmentContainerProp->AddProperty(segmentTexProp);

      dtCore::ArrayActorPropertyBase* segmentPointArrayProp =
         new dtCore::ArrayActorProperty<FenceActor::SegmentPointData>(
         "SegmentPointArray", "Segment Point Array",
         "The list of segment points.",
         dtCore::ArrayActorProperty<FenceActor::SegmentPointData>::SetIndexFuncType(this, &FenceActorProxy::SetSegmentPointIndex),
         dtCore::ArrayActorProperty<FenceActor::SegmentPointData>::GetDefaultFuncType(this, &FenceActorProxy::GetDefaultSegmentPoint),
         dtCore::ArrayActorProperty<FenceActor::SegmentPointData>::GetArrayFuncType(actor, &FenceActor::GetSegmentPointArray),
         dtCore::ArrayActorProperty<FenceActor::SegmentPointData>::SetArrayFuncType(actor, &FenceActor::SetSegmentPointArray),
         segmentContainerProp, "Fence");
      AddProperty(segmentPointArrayProp);

      // Resource ID Array
      dtCore::IntActorProperty* pointIndexProp =
         new dtCore::IntActorProperty(
         "PointIndex", "Point Index",
         dtCore::IntActorProperty::SetFuncType(this, &FenceActorProxy::SetPointIndex),
         dtCore::IntActorProperty::GetFuncType(this, &FenceActorProxy::GetPointIndex),
         "The Index of the point.", "Build");

      dtCore::IntActorProperty* segmentIndexProp =
         new dtCore::IntActorProperty(
         "SegmentIndex", "Segment Index",
         dtCore::IntActorProperty::SetFuncType(this, &FenceActorProxy::SetSegmentIndex),
         dtCore::IntActorProperty::GetFuncType(this, &FenceActorProxy::GetSegmentIndex),
         "The Index of the segment.", "Build");

      dtCore::IntActorProperty* postIDProp =
         new dtCore::IntActorProperty(
         "PostID", "Post Mesh ID",
         dtCore::IntActorProperty::SetFuncType(this, &FenceActorProxy::SetPostID),
         dtCore::IntActorProperty::GetFuncType(this, &FenceActorProxy::GetPostID),
         "The ID of the mesh resource to use as this post.", "Build");

      dtCore::IntActorProperty* segmentIDProp =
         new dtCore::IntActorProperty(
         "SegmentID", "Segment Texture ID",
         dtCore::IntActorProperty::SetFuncType(this, &FenceActorProxy::SetSegmentID),
         dtCore::IntActorProperty::GetFuncType(this, &FenceActorProxy::GetSegmentID),
         "The ID of the texture resource to use as this segment.", "Build");

      dtCore::ContainerActorProperty* resourceIDContainerProp =
         new dtCore::ContainerActorProperty(
         "ResourceIDContainer", "Resource IDs",
         "Stores the resource ID's for posts and segments.",
         "Build");
      resourceIDContainerProp->AddProperty(pointIndexProp);
      resourceIDContainerProp->AddProperty(segmentIndexProp);
      resourceIDContainerProp->AddProperty(postIDProp);
      resourceIDContainerProp->AddProperty(segmentIDProp);

      dtCore::ArrayActorPropertyBase* postIDArrayProp =
         new dtCore::ArrayActorProperty<FenceActor::ResourceIDData>(
         "ResourceIDArray", "Resource ID Array",
         "The list of post ID's",
         dtCore::ArrayActorProperty<FenceActor::ResourceIDData>::SetIndexFuncType(this, &FenceActorProxy::SetResourceIDIndex),
         dtCore::ArrayActorProperty<FenceActor::ResourceIDData>::GetDefaultFuncType(this, &FenceActorProxy::GetDefaultResourceID),
         dtCore::ArrayActorProperty<FenceActor::ResourceIDData>::GetArrayFuncType(actor, &FenceActor::GetResourceIDArray),
         dtCore::ArrayActorProperty<FenceActor::ResourceIDData>::SetArrayFuncType(actor, &FenceActor::SetResourceIDArray),
         resourceIDContainerProp, "Build");
      AddProperty(postIDArrayProp);

      // Minimum Post Distance
      dtCore::FloatActorProperty* postMinDistanceProp =
         new dtCore::FloatActorProperty(
         "PostMinDistance", "Post Min Distance",
         dtCore::FloatActorProperty::SetFuncType(actor, &FenceActor::SetPostMinDistance),
         dtCore::FloatActorProperty::GetFuncType(actor, &FenceActor::GetPostMinDistance),
         "Sets the minimum distance between each post.", "Fence");
      AddProperty(postMinDistanceProp);

      // Maximum Post Distance
      dtCore::FloatActorProperty* postMaxDistanceProp =
         new dtCore::FloatActorProperty(
         "PostMaxDistance", "Post Max Distance",
         dtCore::FloatActorProperty::SetFuncType(actor, &FenceActor::SetPostMaxDistance),
         dtCore::FloatActorProperty::GetFuncType(actor, &FenceActor::GetPostMaxDistance),
         "Sets the maximum distance between each post.", "Fence");
      AddProperty(postMaxDistanceProp);

      // Post Scale
      dtCore::FloatActorProperty* fenceScale =
         new dtCore::FloatActorProperty(
         "FenceScale", "Fence Scale",
         dtCore::FloatActorProperty::SetFuncType(actor, &FenceActor::SetFenceScale),
         dtCore::FloatActorProperty::GetFuncType(actor, &FenceActor::GetFenceScale),
         "Sets the scale of the fence.", "Fence");
      AddProperty(fenceScale);

      // Segment Height
      dtCore::FloatActorProperty* segmentHeightProp =
         new dtCore::FloatActorProperty(
         "SegmentHeight", "Segment Height",
         dtCore::FloatActorProperty::SetFuncType(actor, &FenceActor::SetSegmentHeight),
         dtCore::FloatActorProperty::GetFuncType(actor, &FenceActor::GetSegmentHeight),
         "Sets the segment height.", "Fence");
      AddProperty(segmentHeightProp);

      // Segment Width
      dtCore::FloatActorProperty* segmentWidthProp =
         new dtCore::FloatActorProperty(
         "SegmentWidth", "Segment Width",
         dtCore::FloatActorProperty::SetFuncType(actor, &FenceActor::SetSegmentWidth),
         dtCore::FloatActorProperty::GetFuncType(actor, &FenceActor::GetSegmentWidth),
         "Sets the segment width.", "Fence");
      AddProperty(segmentWidthProp);

      // Top Texture Ratio.
      dtCore::FloatActorProperty* topTextureRatioProp =
         new dtCore::FloatActorProperty(
         "TopTextureRatio", "Texture Ratio",
         dtCore::FloatActorProperty::SetFuncType(actor, &FenceActor::SetTopTextureRatio),
         dtCore::FloatActorProperty::GetFuncType(actor, &FenceActor::GetTopTextureRatio),
         "The amount of the segment texture (from top to bottom) to use for the top of the fence (value 0-1).",
         "Fence");
      AddProperty(topTextureRatioProp);

      // Scale
      dtCore::Vec3ActorProperty* scaleProp =
         new dtCore::Vec3ActorProperty(
         "Scale", "Scale",
         dtCore::Vec3ActorProperty::SetFuncType(actor, &FenceActor::SetScale),
         dtCore::Vec3ActorProperty::GetFuncType(actor, &FenceActor::GetScale),
         "Sets the post max distance (x), segment width (y), and fence scale (z).",
         "Build");
      AddProperty(scaleProp);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetPostResourceIndex(int index)
   {
      mPostResourceIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentResourceIndex(int index)
   {
      mSegmentResourceIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor FenceActorProxy::GetDefaultPost(void)
   {
      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor FenceActorProxy::GetDefaultSegment(void)
   {
      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetPostMesh(const dtCore::ResourceDescriptor& value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<dtCore::ResourceDescriptor> postArray = actor->GetPostResourceArray();
      if (mPostResourceIndex >= 0 && mPostResourceIndex < (int)postArray.size())
      {
         postArray[mPostResourceIndex] = value;
         actor->SetPostResourceArray(postArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor FenceActorProxy::GetPostMesh()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<dtCore::ResourceDescriptor> postArray = actor->GetPostResourceArray();
      if (mPostResourceIndex >= 0 && mPostResourceIndex < (int)postArray.size())
      {
         return postArray[mPostResourceIndex];
      }

      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentTexture(const dtCore::ResourceDescriptor& value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<dtCore::ResourceDescriptor> segmentArray = actor->GetSegmentResourceArray();
      if (mSegmentResourceIndex >= 0 && mSegmentResourceIndex < (int)segmentArray.size())
      {
         segmentArray[mSegmentResourceIndex] = value;
         actor->SetSegmentResourceArray(segmentArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor FenceActorProxy::GetSegmentTexture()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<dtCore::ResourceDescriptor> segmentArray = actor->GetSegmentResourceArray();
      if (mSegmentResourceIndex >= 0 && mSegmentResourceIndex < (int)segmentArray.size())
      {
         return segmentArray[mSegmentResourceIndex];
      }

      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetResourceIDIndex(int index)
   {
      mResourceIDIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   FenceActor::ResourceIDData FenceActorProxy::GetDefaultResourceID()
   {
      return FenceActor::ResourceIDData();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentPointIndex(int index)
   {
      mSegmentPointIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   FenceActor::SegmentPointData FenceActorProxy::GetDefaultSegmentPoint()
   {
      return FenceActor::SegmentPointData();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int FenceActorProxy::GetPointIndex()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         return resourceArray[mResourceIDIndex].pointIndex;
      }

      return GetDefaultResourceID().pointIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetPointIndex(int value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         resourceArray[mResourceIDIndex].pointIndex = value;
         actor->SetResourceIDArray(resourceArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int FenceActorProxy::GetSegmentIndex()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         return resourceArray[mResourceIDIndex].segmentIndex;
      }

      return GetDefaultResourceID().segmentIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentIndex(int value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         resourceArray[mResourceIDIndex].segmentIndex = value;
         actor->SetResourceIDArray(resourceArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int FenceActorProxy::GetPostID()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         return resourceArray[mResourceIDIndex].postID;
      }

      return GetDefaultResourceID().postID;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetPostID(int value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         resourceArray[mResourceIDIndex].postID = value;
         actor->SetResourceIDArray(resourceArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int FenceActorProxy::GetSegmentID()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         return resourceArray[mResourceIDIndex].segmentID;
      }

      return GetDefaultResourceID().segmentID;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentID(int value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         resourceArray[mResourceIDIndex].segmentID = value;
         actor->SetResourceIDArray(resourceArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 FenceActorProxy::GetSegmentPoint()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::SegmentPointData> pointArray = actor->GetSegmentPointArray();
      if (mSegmentPointIndex < (int)pointArray.size())
      {
         return pointArray[mSegmentPointIndex].position;
      }

      return osg::Vec2();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentPoint(const osg::Vec2& value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::SegmentPointData> pointArray = actor->GetSegmentPointArray();
      if (mSegmentPointIndex < (int)pointArray.size())
      {
         pointArray[mSegmentPointIndex].position = value;
         actor->SetSegmentPointArray(pointArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   float FenceActorProxy::GetSegmentPointTextureHeight()
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::SegmentPointData> pointArray = actor->GetSegmentPointArray();
      if (mSegmentPointIndex < (int)pointArray.size())
      {
         return pointArray[mSegmentPointIndex].textureHeight;
      }

      return -1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentPointTextureHeight(float value)
   {
      FenceActor* actor = NULL;
      GetDrawable(actor);

      std::vector<FenceActor::SegmentPointData> pointArray = actor->GetSegmentPointArray();
      if (mSegmentPointIndex < (int)pointArray.size())
      {
         pointArray[mSegmentPointIndex].textureHeight = value;
         actor->SetSegmentPointArray(pointArray);
      }
   }
}
////////////////////////////////////////////////////////////////////////////////
