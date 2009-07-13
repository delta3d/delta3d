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

////////////////////////////////////////////////////////////////////////////////
// FENCE POST TRANSFORMABLE
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
FencePostTransformable::FencePostTransformable()
   : mRenderingParent(false)
{
}

////////////////////////////////////////////////////////////////////////////////
FencePostTransformable::~FencePostTransformable()
{
   SetSize(0);
}

////////////////////////////////////////////////////////////////////////////////
bool FencePostTransformable::SetIndex(int index)
{
   if (index < 0) index = 0;

   // Expand the size.
   while (index >= (int)mGeomList.size())
   {
      if (!mOrigin.valid())
      {
         mOrigin = new dtCore::Transformable();
         AddChild(mOrigin.get());
         //osg::Group* myGroup = GetOSGNode()->asGroup();
         //myGroup->addChild(mOrigin->GetOSGNode());
         dtCore::Transform segmentTransform;
         mOrigin->SetTransform(segmentTransform);
      }

      if (!mSegColorList.valid())
      {
         mSegColorList = new osg::Vec4Array();
         mSegColorList->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      }

      GeomData data;

      // Create the post geometry object.
      data.post = new dtCore::Object();
      if (!data.post.valid()) return false;

      osg::Group* myGroup = GetOSGNode()->asGroup();
      if (!myGroup) return false;
      myGroup->addChild(data.post->GetOSGNode());

      // Create Geometry objects to store all the segment quads.
      data.segGeom = new osg::Geometry();
      if (!data.segGeom.valid()) return false;

      data.segGeode = new osg::Geode();
      if (!data.segGeode.valid()) return false;

      // Add the points geometry to the geode.
      data.segGeode->addDrawable(data.segGeom.get());

      // Add the Geode to the transformable node.
      osg::Group* originGroup = mOrigin->GetOSGNode()->asGroup();
      if (!originGroup) return false;

      originGroup->addChild(data.segGeode.get());

      // Create all of our buffer arrays.
      data.segVertexList = new osg::Vec3Array();
      if (!data.segVertexList.valid()) return false;
      data.segTextureList = new osg::Vec2Array();
      if (!data.segTextureList.valid()) return false;
      data.segNormalList = new osg::Vec3Array();
      if (!data.segNormalList.valid()) return false;

      // Set the size of the buffer arrays.
      data.segVertexList->resize(12);
      data.segTextureList->resize(12);
      data.segNormalList->resize(3);

      // Apply the buffer arrays to the geometry.
      data.segGeom->setVertexArray(data.segVertexList.get());

      data.segGeom->setTexCoordArray(0, data.segTextureList.get());

      data.segGeom->setColorArray(mSegColorList.get());
      data.segGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

      data.segGeom->setNormalArray(data.segNormalList.get());
      data.segGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

      // Now Set the primitive sets for the three sides of the segment.
      data.segGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
      data.segGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 4, 4));
      data.segGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 8, 4));

      // Create a texture object to use for the segment.
      data.segTexture = new osg::Texture2D();
      if (!data.segTexture.valid()) return false;

      data.segTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);

      osg::StateSet* stateset = data.segGeom->getOrCreateStateSet();
      if (!stateset) return false;

      stateset->setTextureAttributeAndModes(0, data.segTexture.get(), osg::StateAttribute::ON);

      mGeomList.push_back(data);
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool FencePostTransformable::SetSize(int size)
{
   if (size < 0) size = 0;

   // Shrink the size.
   if (size <= (int)mGeomList.size())
   {
      while ((int)mGeomList.size() > size)
      {
         GeomData& data = mGeomList.back();

         osg::Group* myGroup = GetOSGNode()->asGroup();
         if (!myGroup) return false;
         myGroup->removeChild(data.post->GetOSGNode());
         data.post = NULL;

         osg::Group* originGroup = mOrigin->GetOSGNode()->asGroup();
         if (!originGroup) return false;
         originGroup->removeChild(data.segGeode.get());

         data.segGeode->removeDrawable(data.segGeom.get());
         data.segGeode = NULL;

         data.segGeom->setVertexArray(NULL);
         data.segGeom->setTexCoordArray(0, NULL);
         data.segGeom->setColorArray(NULL);
         data.segGeom->setNormalArray(NULL);
         data.segGeom->removePrimitiveSet(0, 3);
         data.segGeom = NULL;

         data.segVertexList = NULL;
         data.segTextureList = NULL;
         data.segNormalList = NULL;

         data.segTexture = NULL;

         mGeomList.pop_back();
      }
   }
   // Expand the size.
   else
   {
      return SetIndex(size - 1);
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void FencePostTransformable::ClearParentGeometry()
{
   // If it ever gets here, it means we are ready to draw this
   // post properly, so remove any geometry from it's parents.
   if (mRenderingParent)
   {
      // First clear the old visualization data from the point.
      while (GetNumChildren())
      {
         RemoveChild(GetChild(0));
      }

      osg::Group* pointGroup = GetOSGNode()->asGroup();
      pointGroup->removeChildren(0, pointGroup->getNumChildren());

      mRenderingParent = false;

      // Just for good measure, make sure the Geom List is also empty.
      SetSize(0);
   }
}


/////////////////////////////////////////////////////////////////////////////
// ACTOR CODE
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
FenceActor::FenceActor(dtDAL::ActorProxy* proxy, const std::string& name)
   : BaseClass(proxy, name)
   , mPostMinDistance(1.0f)
   , mPostMaxDistance(10.0f)
   , mFenceScale(1.0f)
   , mSegmentHeight(5.0f)
   , mSegmentWidth(1.0f)
   , mTopTextureRatio(0.2f)
{
   RemovePoint(0);
   AddPoint(osg::Vec3());
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
   if (!IsMapLoaded())
   {
      return;
   }

   // If there are no meshes setup for the posts yet, use the default sphere/line schema.
   if (mPostResourceList.size() == 0)
   {
      BaseClass::Visualize(pointIndex);

      FencePostTransformable* fencePost = dynamic_cast<FencePostTransformable*>(GetPointDrawable(pointIndex));
      if (fencePost) fencePost->mRenderingParent = true;
      return;
   }

   dtCore::Transformable* point = GetPointDrawable(pointIndex);
   if (point)
   {
      FencePostTransformable* fencePost = dynamic_cast<FencePostTransformable*>(GetPointDrawable(pointIndex));
      if (fencePost)
      {
         // First make sure our parent geometry is cleared properly.
         fencePost->ClearParentGeometry();

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
            while(true)
            {
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
                  PlaceSegment(pointIndex, subPostIndex - 1, prevPostTransform, postTransform, texLength);

                  // Now make sure we clean up any additional indices.
                  fencePost->SetSize(subPostIndex);
                  break;
               }

               // Place a post if we are not too close to the end.
               if (totalLength - length > mPostMinDistance)
               {
                  postTransform = PlacePost(pointIndex, subPostIndex, position);
               }

               // Now place a segment between the current post and the previous.
               PlaceSegment(pointIndex, subPostIndex - 1, prevPostTransform, postTransform, 1.0f);
               subPostIndex++;

               lastPost = position;
               prevPostTransform = postTransform;
            }
         }
         // Make sure we clear any geometry on the last post.
         else
         {
            fencePost->SetSize(1);

            prevPostTransform = PlacePost(pointIndex, 0, postPos);
            PlaceSegment(pointIndex, 0, prevPostTransform, prevPostTransform, 1.0f);
         }
      }
   }
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
std::vector<std::vector<FenceActor::ResourceIDData> > FenceActor::GetResourceIDArray()
{
   return mResourceIDList;
}

////////////////////////////////////////////////////////////////////////////////
void FenceActor::SetResourceIDArray(const std::vector<std::vector<FenceActor::ResourceIDData> >& value)
{
   mResourceIDList = value;

   Visualize();
}

////////////////////////////////////////////////////////////////////////////////
std::string FenceActor::GetPostMesh(int pointIndex, int subIndex)
{
   // First check if there is already a predefined mesh index to use for this post.
   int meshIndex = 0;
   if (pointIndex < (int)mResourceIDList.size())
   {
      if (subIndex < (int)mResourceIDList[pointIndex].size())
      {
         meshIndex = mResourceIDList[pointIndex][subIndex].postID;
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
   // Make sure the resource lists are large enough.
   if (pointIndex >= (int)mResourceIDList.size())
   {
      SetResourceIDSize(pointIndex);
   }

   if (subIndex >= (int)mResourceIDList[pointIndex].size())
   {
      SetResourceIDSubSize(pointIndex, subIndex);
   }

   // Now set the mesh.
   mResourceIDList[pointIndex][subIndex].postID = meshIndex;
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
   // First check if there is already a predefined mesh index to use for this post.
   int textureIndex = 0;
   if (pointIndex < (int)mResourceIDList.size())
   {
      if (subIndex < (int)mResourceIDList[pointIndex].size())
      {
         textureIndex = mResourceIDList[pointIndex][subIndex].segmentID;
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
   return osg::Vec3(mPostMaxDistance, mSegmentWidth, mFenceScale);
}

////////////////////////////////////////////////////////////////////////////////
void FenceActor::SetScale(const osg::Vec3& value)
{
   mPostMaxDistance = value.x();
   if (mPostMaxDistance <= 1.0f) mPostMaxDistance = 1.0f;
   if (mPostMaxDistance < mPostMinDistance) mPostMaxDistance = mPostMinDistance;

   mSegmentWidth = value.y();
   if (mSegmentWidth < 0.0f) mSegmentWidth = 0.0f;

   mFenceScale = value.z();

   Visualize();
}

////////////////////////////////////////////////////////////////////////////////
dtCore::Transformable* FenceActor::CreatePointDrawable(osg::Vec3 position)
{
   dtCore::Transformable* point = new FencePostTransformable();

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
void FenceActor::SetResourceIDSize(int pointCount)
{
   if (pointCount != (int)mResourceIDList.size() - 1)
   {
      mResourceIDList.resize(pointCount, std::vector<FenceActor::ResourceIDData>());
   }
}

////////////////////////////////////////////////////////////////////////////////
void FenceActor::SetResourceIDSubSize(int pointIndex, int subCount)
{
   if (pointIndex < (int)mResourceIDList.size())
   {
      SetResourceIDSize(pointIndex);
   }

   mResourceIDList[pointIndex].resize(subCount, FenceActor::ResourceIDData());
}

////////////////////////////////////////////////////////////////////////////////
dtCore::Transform FenceActor::PlacePost(int pointIndex, int subIndex, osg::Vec3 position)
{
   dtCore::Transform transform;
   transform.SetTranslation(position);

   dtCore::Transformable* point = GetPointDrawable(pointIndex);
   if (point)
   {
      FencePostTransformable* fencePost = dynamic_cast<FencePostTransformable*>(point);
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

            dtCore::RefPtr<dtCore::Object> postMesh = fencePost->mGeomList[subIndex].post;

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
      FencePostTransformable* fencePost = dynamic_cast<FencePostTransformable*>(point);
      if (fencePost)
      {
         if (!fencePost->SetIndex(subIndex))
         {
            return;
         }

         // Always reset the origin transform to the origin of the world.
         dtCore::Transform segmentTransform;
         fencePost->mOrigin->SetTransform(segmentTransform);

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

         dtCore::Transform transform;
         fencePost->mOrigin->SetTransform(transform);

         ////////////////////////////////////////////////////////////////////////////////
         // Vertex
         // Front Wall
         /// Bottom left vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(0) = (start.GetTranslation() + startWidth);
         /// Top left vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(1) = ((start.GetTranslation() + startWidth) + (startUp * mSegmentHeight * mFenceScale));
         /// Top right vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(2) = ((end.GetTranslation() + endWidth) + (endUp * mSegmentHeight * mFenceScale));
         /// Bottom right vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(3) = (end.GetTranslation() + endWidth);

         // Back Wall
         /// Bottom left vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(4) = (end.GetTranslation() - endWidth);
         /// Top left vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(5) = ((end.GetTranslation() - endWidth) + (endUp * mSegmentHeight * mFenceScale));
         /// Top right vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(6) = ((start.GetTranslation() - startWidth) + (startUp * mSegmentHeight * mFenceScale));
         /// Bottom right vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(7) = (start.GetTranslation() - startWidth);

         // Top Wall
         /// Bottom left vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(8) = ((start.GetTranslation() + startWidth) + (startUp * mSegmentHeight * mFenceScale));
         /// Top left vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(9) = ((start.GetTranslation() - startWidth) + (startUp * mSegmentHeight * mFenceScale));
         /// Top right vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(10)= ((end.GetTranslation() - endWidth) + (endUp * mSegmentHeight * mFenceScale));
         /// Bottom right vertex.
         fencePost->mGeomList[subIndex].segVertexList->at(11)= ((end.GetTranslation() + endWidth) + (endUp * mSegmentHeight * mFenceScale));

         ////////////////////////////////////////////////////////////////////////////////
         // Texture
         // Front Wall
         fencePost->mGeomList[subIndex].segTextureList->at(0) = (osg::Vec2(1.0f, 0.0f));
         fencePost->mGeomList[subIndex].segTextureList->at(1) = (osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
         fencePost->mGeomList[subIndex].segTextureList->at(2) = (osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));
         fencePost->mGeomList[subIndex].segTextureList->at(3) = (osg::Vec2(1.0f - length, 0.0f));

         // Back Wall
         fencePost->mGeomList[subIndex].segTextureList->at(4) = (osg::Vec2(1.0f - length, 0.0f));
         fencePost->mGeomList[subIndex].segTextureList->at(5) = (osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));
         fencePost->mGeomList[subIndex].segTextureList->at(6) = (osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
         fencePost->mGeomList[subIndex].segTextureList->at(7) = (osg::Vec2(1.0f, 0.0f));

         // Top Wall
         fencePost->mGeomList[subIndex].segTextureList->at(8) = (osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
         fencePost->mGeomList[subIndex].segTextureList->at(9) = (osg::Vec2(1.0f, 1.0f));
         fencePost->mGeomList[subIndex].segTextureList->at(10)= (osg::Vec2(1.0f - length, 1.0f));
         fencePost->mGeomList[subIndex].segTextureList->at(11)= (osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));

         ////////////////////////////////////////////////////////////////////////////////
         // Normal
         fencePost->mGeomList[subIndex].segNormalList->at(0) = dir;
         fencePost->mGeomList[subIndex].segNormalList->at(1) = dir;
         fencePost->mGeomList[subIndex].segNormalList->at(2) = dir;

         // Get the texture to use for this segment.
         std::string textureName = GetSegmentTexture(pointIndex, subIndex);
         if (!textureName.empty())
         {
            // set up the texture state.
            dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(textureName);
            fencePost->mGeomList[subIndex].segTexture->setImage(osgDB::readImageFile(dtDAL::Project::GetInstance().GetResourcePath(descriptor)));
         }

         // Make sure the geometry node knows to re-calculate the bounding area and display lists.
         fencePost->mGeomList[subIndex].segGeom->dirtyBound();
         fencePost->mGeomList[subIndex].segGeom->dirtyDisplayList();
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
   , mResourceIDSubIndex(0)
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
   SetActor(*new FenceActor(this));
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
   resourceIDContainerProp->AddProperty(postIDProp);
   resourceIDContainerProp->AddProperty(segmentIDProp);

   dtDAL::ArrayActorPropertyBase* subPostIDArrayProp =
      new dtDAL::ArrayActorProperty<FenceActor::ResourceIDData>(
      "SubResourceIDArray", "Sub Resource ID Array",
      "The list of sub post ID's",
      dtDAL::MakeFunctor(*this, &FenceActorProxy::SetResourceIDSubIndex),
      dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetDefaultSubResourceID),
      dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetSubResourceIDArray),
      dtDAL::MakeFunctor(*this, &FenceActorProxy::SetSubResourceIDArray),
      resourceIDContainerProp, "Build");

   dtDAL::ArrayActorPropertyBase* postIDArrayProp =
      new dtDAL::ArrayActorProperty<std::vector<FenceActor::ResourceIDData> >(
      "ResourceIDArray", "Resource ID Array",
      "The list of post ID's",
      dtDAL::MakeFunctor(*this, &FenceActorProxy::SetResourceIDIndex),
      dtDAL::MakeFunctorRet(*this, &FenceActorProxy::GetDefaultResourceID),
      dtDAL::MakeFunctorRet(*actor, &FenceActor::GetResourceIDArray),
      dtDAL::MakeFunctor(*actor, &FenceActor::SetResourceIDArray),
      subPostIDArrayProp, "Build");
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
void FenceActorProxy::SetResourceIDSubIndex(int index)
{
   mResourceIDSubIndex = index;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<FenceActor::ResourceIDData> FenceActorProxy::GetDefaultResourceID()
{
   return std::vector<FenceActor::ResourceIDData>();
}

////////////////////////////////////////////////////////////////////////////////
FenceActor::ResourceIDData FenceActorProxy::GetDefaultSubResourceID()
{
   return FenceActor::ResourceIDData();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<FenceActor::ResourceIDData> FenceActorProxy::GetSubResourceIDArray()
{
   FenceActor* actor = NULL;
   GetActor(actor);

   std::vector<std::vector<FenceActor::ResourceIDData> > postArray = actor->GetResourceIDArray();

   if (mResourceIDIndex < (int)postArray.size())
   {
      return postArray[mResourceIDIndex];
   }

   return GetDefaultResourceID();
}

////////////////////////////////////////////////////////////////////////////////
void FenceActorProxy::SetSubResourceIDArray(const std::vector<FenceActor::ResourceIDData>& value)
{
   FenceActor* actor = NULL;
   GetActor(actor);

   std::vector<std::vector<FenceActor::ResourceIDData> > postArray = actor->GetResourceIDArray();

   if (mResourceIDIndex < (int)postArray.size())
   {
      postArray[mResourceIDIndex] = value;
      actor->SetResourceIDArray(postArray);
   }
}

////////////////////////////////////////////////////////////////////////////////
int FenceActorProxy::GetPostID()
{
   FenceActor* actor = NULL;
   GetActor(actor);

   std::vector<std::vector<FenceActor::ResourceIDData> > postArray = actor->GetResourceIDArray();
   if (mResourceIDIndex < (int)postArray.size())
   {
      if (mResourceIDSubIndex < (int)postArray[mResourceIDIndex].size())
      {
         return postArray[mResourceIDIndex][mResourceIDSubIndex].postID;
      }
   }

   return GetDefaultSubResourceID().postID;
}

////////////////////////////////////////////////////////////////////////////////
void FenceActorProxy::SetPostID(int value)
{
   FenceActor* actor = NULL;
   GetActor(actor);

   std::vector<std::vector<FenceActor::ResourceIDData> > postArray = actor->GetResourceIDArray();
   if (mResourceIDIndex < (int)postArray.size())
   {
      if (mResourceIDSubIndex < (int)postArray[mResourceIDIndex].size())
      {
         postArray[mResourceIDIndex][mResourceIDSubIndex].postID = value;
         actor->SetResourceIDArray(postArray);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
int FenceActorProxy::GetSegmentID()
{
   FenceActor* actor = NULL;
   GetActor(actor);

   std::vector<std::vector<FenceActor::ResourceIDData> > postArray = actor->GetResourceIDArray();
   if (mResourceIDIndex < (int)postArray.size())
   {
      if (mResourceIDSubIndex < (int)postArray[mResourceIDIndex].size())
      {
         return postArray[mResourceIDIndex][mResourceIDSubIndex].segmentID;
      }
   }

   return GetDefaultSubResourceID().segmentID;
}

////////////////////////////////////////////////////////////////////////////////
void FenceActorProxy::SetSegmentID(int value)
{
   FenceActor* actor = NULL;
   GetActor(actor);

   std::vector<std::vector<FenceActor::ResourceIDData> > postArray = actor->GetResourceIDArray();
   if (mResourceIDIndex < (int)postArray.size())
   {
      if (mResourceIDSubIndex < (int)postArray[mResourceIDIndex].size())
      {
         postArray[mResourceIDIndex][mResourceIDSubIndex].segmentID = value;
         actor->SetResourceIDArray(postArray);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
