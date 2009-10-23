#include <dtActors/fenceactor.h>
#include <dtCore/object.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>
#include <dtDAL/resourcedescriptor.h>
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

      mSegVertexList->resize(12);
      mSegTextureList->resize(12);
      mSegNormalList->resize(3);

      // Apply the buffer arrays to the geometry.
      mSegGeom->setVertexArray(mSegVertexList.get());

      mSegGeom->setTexCoordArray(0, mSegTextureList.get());

      mSegGeom->setColorArray(fenceParent->mSegColorList.get());
      mSegGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

      mSegGeom->setNormalArray(mSegNormalList.get());
      mSegGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

      // Now Set the primitive sets for the three sides of the segment.
      mSegGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
      mSegGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 4, 4));
      mSegGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 8, 4));

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
      mSegGeom->removePrimitiveSet(0, 3);
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
      if (mPostResourceList.size() == 0)
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
                        subIndex = floor(index);

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
   std::vector<std::string> FenceActor::GetPostResourceArray(void) const
   {
      return mPostResourceList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetPostResourceArray(const std::vector<std::string>& value)
   {
      mPostResourceList = value;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> FenceActor::GetSegmentResourceArray(void) const
   {
      return mSegmentResourceList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActor::SetSegmentResourceArray(const std::vector<std::string>& value)
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
         return mPostResourceList[meshIndex];
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
         return mSegmentResourceList[textureIndex];
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
      if (mPostResourceList.size() == 0)
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

               osg::Node* postMeshNode = postMesh->LoadFile(postMeshName);
               postMesh->SetScale(osg::Vec3(mFenceScale, mFenceScale, mFenceScale));

               // Set the position of the object.
               postMesh->GetTransform(transform);
               transform.SetTranslation(position);
               postMesh->SetTransform(transform);
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
            osg::Vec3 startWidth = startUp ^ dir;
            startWidth.normalize();
            startWidth *= mSegmentWidth * 0.5f * mFenceScale;

            osg::Vec3 endWidth = endUp ^ dir;
            endWidth.normalize();
            endWidth *= mSegmentWidth * 0.5f * mFenceScale;

            ////////////////////////////////////////////////////////////////////////////////
            // Vertex
            // Front Wall
            /// Bottom left vertex.
            geomData->mSegVertexList->at(0) = (start.GetTranslation() + startWidth);
            /// Top left vertex.
            geomData->mSegVertexList->at(1) = ((start.GetTranslation() + startWidth) + (startUp * mSegmentHeight * mFenceScale));
            /// Top right vertex.
            geomData->mSegVertexList->at(2) = ((end.GetTranslation() + endWidth) + (endUp * mSegmentHeight * mFenceScale));
            /// Bottom right vertex.
            geomData->mSegVertexList->at(3) = (end.GetTranslation() + endWidth);

            // Back Wall
            /// Bottom left vertex.
            geomData->mSegVertexList->at(4) = (end.GetTranslation() - endWidth);
            /// Top left vertex.
            geomData->mSegVertexList->at(5) = ((end.GetTranslation() - endWidth) + (endUp * mSegmentHeight * mFenceScale));
            /// Top right vertex.
            geomData->mSegVertexList->at(6) = ((start.GetTranslation() - startWidth) + (startUp * mSegmentHeight * mFenceScale));
            /// Bottom right vertex.
            geomData->mSegVertexList->at(7) = (start.GetTranslation() - startWidth);

            // Top Wall
            /// Bottom left vertex.
            geomData->mSegVertexList->at(8) = ((start.GetTranslation() + startWidth) + (startUp * mSegmentHeight * mFenceScale));
            /// Top left vertex.
            geomData->mSegVertexList->at(9) = ((start.GetTranslation() - startWidth) + (startUp * mSegmentHeight * mFenceScale));
            /// Top right vertex.
            geomData->mSegVertexList->at(10)= ((end.GetTranslation() - endWidth) + (endUp * mSegmentHeight * mFenceScale));
            /// Bottom right vertex.
            geomData->mSegVertexList->at(11)= ((end.GetTranslation() + endWidth) + (endUp * mSegmentHeight * mFenceScale));

            ////////////////////////////////////////////////////////////////////////////////
            // Texture
            // Front Wall
            geomData->mSegTextureList->at(0) = (osg::Vec2(1.0f, 0.0f));
            geomData->mSegTextureList->at(1) = (osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
            geomData->mSegTextureList->at(2) = (osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));
            geomData->mSegTextureList->at(3) = (osg::Vec2(1.0f - length, 0.0f));

            // Back Wall
            geomData->mSegTextureList->at(4) = (osg::Vec2(1.0f - length, 0.0f));
            geomData->mSegTextureList->at(5) = (osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));
            geomData->mSegTextureList->at(6) = (osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
            geomData->mSegTextureList->at(7) = (osg::Vec2(1.0f, 0.0f));

            // Top Wall
            geomData->mSegTextureList->at(8) = (osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
            geomData->mSegTextureList->at(9) = (osg::Vec2(1.0f, 1.0f));
            geomData->mSegTextureList->at(10)= (osg::Vec2(1.0f - length, 1.0f));
            geomData->mSegTextureList->at(11)= (osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));

            ////////////////////////////////////////////////////////////////////////////////
            // Normal
            geomData->mSegNormalList->at(0) = startWidth;
            geomData->mSegNormalList->at(1) = -startWidth;
            geomData->mSegNormalList->at(2) = startUp;

            // Get the texture to use for this segment.
            std::string textureName = GetSegmentTexture(pointIndex, subIndex);
            if (!textureName.empty())
            {
               // set up the texture state.
               dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(textureName);
               geomData->mSegTexture->setImage(osgDB::readImageFile(dtDAL::Project::GetInstance().GetResourcePath(descriptor)));
            }

            // Make sure the geometry node knows to re-calculate the bounding area and display lists.
            geomData->mSegGeom->dirtyBound();
            geomData->mSegGeom->dirtyDisplayList();
         }
      }
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
   void FenceActorProxy::CreateActor()
   {
      LinkedPointsActor* actor = new FenceActor(this);
      SetActor(*actor);
      actor->Initialize();
   }

   /////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      FenceActor* actor = NULL;
      GetActor(actor);

      // Post Resources Array
      dtDAL::ResourceActorProperty* postResourceProp =
         new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::STATIC_MESH,
         "PostMeshResource", "Post Mesh",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetPostMesh),
         "Defines the mesh used to represent a fence post.", "Fence");

      dtDAL::ArrayActorPropertyBase* postResourceArrayProp =
         new dtDAL::ArrayActorProperty<std::string>(
         "PostMeshArray", "Post Meshes",
         "Defines the meshes used to represent the fence posts.",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetPostResourceIndex),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetDefaultPost),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetPostResourceArray),
         dtDAL::MakeFunctor(*actor, &FenceActor::SetPostResourceArray),
         postResourceProp, "Fence");
      AddProperty(postResourceArrayProp);

      // Segment Resources Array
      dtDAL::ResourceActorProperty* segmentResourceProp =
         new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
         "SegmentTextureResource", "Segment Texture",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetSegmentTexture),
         "Defines the texture used to represent a fence segment.", "Fence");

      dtDAL::ArrayActorPropertyBase* segmentResourceArrayProp =
         new dtDAL::ArrayActorProperty<std::string>(
         "SegmentTextureArray", "Segment Textures",
         "Defines the textures used to represent the fence segments.",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetSegmentResourceIndex),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetDefaultSegment),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetSegmentResourceArray),
         dtDAL::MakeFunctor(*actor, &FenceActor::SetSegmentResourceArray),
         segmentResourceProp, "Fence");
      AddProperty(segmentResourceArrayProp);

      // Resource ID Array
      dtDAL::IntActorProperty* pointIndexProp =
         new dtDAL::IntActorProperty(
         "PointIndex", "Point Index",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetPointIndex),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetPointIndex),
         "The Index of the point.", "Build");

      dtDAL::IntActorProperty* segmentIndexProp =
         new dtDAL::IntActorProperty(
         "SegmentIndex", "Segment Index",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetSegmentIndex),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetSegmentIndex),
         "The Index of the segment.", "Build");

      dtDAL::IntActorProperty* postIDProp =
         new dtDAL::IntActorProperty(
         "PostID", "Post Mesh ID",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetPostID),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetPostID),
         "The ID of the mesh resource to use as this post.", "Build");

      dtDAL::IntActorProperty* segmentIDProp =
         new dtDAL::IntActorProperty(
         "SegmentID", "Segment Texture ID",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetSegmentID),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetSegmentID),
         "The ID of the texture resource to use as this segment.", "Build");

      dtDAL::ContainerActorProperty* resourceIDContainerProp =
         new dtDAL::ContainerActorProperty(
         "ResourceIDContainer", "Resource IDs",
         "Stores the resource ID's for posts and segments.",
         "Build");
      resourceIDContainerProp->AddProperty(pointIndexProp);
      resourceIDContainerProp->AddProperty(segmentIndexProp);
      resourceIDContainerProp->AddProperty(postIDProp);
      resourceIDContainerProp->AddProperty(segmentIDProp);

      dtDAL::ArrayActorPropertyBase* postIDArrayProp =
         new dtDAL::ArrayActorProperty<FenceActor::ResourceIDData>(
         "ResourceIDArray", "Resource ID Array",
         "The list of post ID's",
         dtDAL::MakeFunctor(*this, &FenceActorProxy::SetResourceIDIndex),
         dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetDefaultResourceID),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetResourceIDArray),
         dtDAL::MakeFunctor(*actor, &FenceActor::SetResourceIDArray),
         resourceIDContainerProp, "Build");
      AddProperty(postIDArrayProp);

      // Minimum Post Distance
      dtDAL::FloatActorProperty* postMinDistanceProp =
         new dtDAL::FloatActorProperty(
         "PostMinDistance", "Post Min Distance",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetPostMinDistance),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetPostMinDistance),
         "Sets the minimum distance between each post.", "Fence");
      AddProperty(postMinDistanceProp);

      // Maximum Post Distance
      dtDAL::FloatActorProperty* postMaxDistanceProp =
         new dtDAL::FloatActorProperty(
         "PostMaxDistance", "Post Max Distance",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetPostMaxDistance),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetPostMaxDistance),
         "Sets the maximum distance between each post.", "Fence");
      AddProperty(postMaxDistanceProp);

      // Post Scale
      dtDAL::FloatActorProperty* fenceScale =
         new dtDAL::FloatActorProperty(
         "FenceScale", "Fence Scale",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetFenceScale),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetFenceScale),
         "Sets the scale of the fence.", "Fence");
      AddProperty(fenceScale);

      // Segment Height
      dtDAL::FloatActorProperty* segmentHeightProp =
         new dtDAL::FloatActorProperty(
         "SegmentHeight", "Segment Height",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetSegmentHeight),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetSegmentHeight),
         "Sets the segment height.", "Fence");
      AddProperty(segmentHeightProp);

      // Segment Width
      dtDAL::FloatActorProperty* segmentWidthProp =
         new dtDAL::FloatActorProperty(
         "SegmentWidth", "Segment Width",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetSegmentWidth),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetSegmentWidth),
         "Sets the segment width.", "Fence");
      AddProperty(segmentWidthProp);

      // Top Texture Ratio.
      dtDAL::FloatActorProperty* topTextureRatioProp =
         new dtDAL::FloatActorProperty(
         "TopTextureRatio", "Texture Ratio",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetTopTextureRatio),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetTopTextureRatio),
         "The amount of the segment texture (from top to bottom) to use for the top of the fence (value 0-1).",
         "Fence");
      AddProperty(topTextureRatioProp);

      // Scale
      dtDAL::Vec3ActorProperty* scaleProp =
         new dtDAL::Vec3ActorProperty(
         "Scale", "Scale",
         dtDAL::MakeFunctor(*actor, &FenceActor::SetScale),
         dtDAL::MakeFunctorRet(*actor, &FenceActor::GetScale),
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
   std::string FenceActorProxy::GetDefaultPost(void)
   {
      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string FenceActorProxy::GetDefaultSegment(void)
   {
      return "";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor* FenceActorProxy::GetResource(const std::string& name)
   {
      static dtDAL::ResourceDescriptor descriptor;

      FenceActor* actor = NULL;
      GetActor(actor);

      if (name == "PostMeshResource")
      {
         std::vector<std::string> postArray = actor->GetPostResourceArray();
         if (mPostResourceIndex >= 0 && mPostResourceIndex < (int)postArray.size())
         {
            descriptor = dtDAL::ResourceDescriptor(postArray[mPostResourceIndex]);
            return &descriptor;
         }
      }
      else if (name == "SegmentTextureResource")
      {
         std::vector<std::string> segmentArray = actor->GetSegmentResourceArray();
         if (mSegmentResourceIndex >= 0 && mSegmentResourceIndex < (int)segmentArray.size())
         {
            descriptor = dtDAL::ResourceDescriptor(segmentArray[mSegmentResourceIndex]);
            return &descriptor;
         }
      }

      return ActorProxy::GetResource(name);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const dtDAL::ResourceDescriptor* FenceActorProxy::GetResource(const std::string& name) const
   {
      static dtDAL::ResourceDescriptor descriptor;

      const FenceActor* actor = NULL;
      GetActor(actor);

      if (name == "PostMeshResource")
      {
         std::vector<std::string> postArray = actor->GetPostResourceArray();
         if (mPostResourceIndex >= 0 && mPostResourceIndex < (int)postArray.size())
         {
            descriptor = dtDAL::ResourceDescriptor(postArray[mPostResourceIndex]);
            return &descriptor;
         }
      }
      else if (name == "SegmentTextureResource")
      {
         std::vector<std::string> segmentArray = actor->GetSegmentResourceArray();
         if (mSegmentResourceIndex >= 0 && mSegmentResourceIndex < (int)segmentArray.size())
         {
            descriptor = dtDAL::ResourceDescriptor(segmentArray[mSegmentResourceIndex]);
            return &descriptor;
         }
      }

      return ActorProxy::GetResource(name);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetPostMesh(const std::string& fileName)
   {
      FenceActor* actor = NULL;
      GetActor(actor);

      std::vector<std::string> postArray = actor->GetPostResourceArray();
      if (mPostResourceIndex >= 0 && mPostResourceIndex < (int)postArray.size())
      {
         postArray[mPostResourceIndex] = fileName;
         actor->SetPostResourceArray(postArray);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FenceActorProxy::SetSegmentTexture(const std::string& fileName)
   {
      FenceActor* actor = NULL;
      GetActor(actor);

      std::vector<std::string> segmentArray = actor->GetSegmentResourceArray();
      if (mSegmentResourceIndex >= 0 && mSegmentResourceIndex < (int)segmentArray.size())
      {
         segmentArray[mSegmentResourceIndex] = fileName;
         actor->SetSegmentResourceArray(segmentArray);
      }
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
   int FenceActorProxy::GetPointIndex()
   {
      FenceActor* actor = NULL;
      GetActor(actor);

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
      GetActor(actor);

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
      GetActor(actor);

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
      GetActor(actor);

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
      GetActor(actor);

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
      GetActor(actor);

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
      GetActor(actor);

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
      GetActor(actor);

      std::vector<FenceActor::ResourceIDData> resourceArray = actor->GetResourceIDArray();
      if (mResourceIDIndex < (int)resourceArray.size())
      {
         resourceArray[mResourceIDIndex].segmentID = value;
         actor->SetResourceIDArray(resourceArray);
      }
   }
}
////////////////////////////////////////////////////////////////////////////////
