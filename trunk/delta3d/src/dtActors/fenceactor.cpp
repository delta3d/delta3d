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
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osgDB/ReadFile>

/////////////////////////////////////////////////////////////////////////////
// ACTOR CODE
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
FenceActor::FenceActor(const std::string& name)
   : BaseClass(name)
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
   // Iterate through each point.
   for (int pointIndex = 0; pointIndex < GetPointCount(); pointIndex++)
   {
      Visualize(pointIndex);
   }
}

////////////////////////////////////////////////////////////////////////////////
void FenceActor::Visualize(int pointIndex)
{
   // If there are no meshes setup for the posts yet, use the default sphere/line schema.
   if (mPostResourceList.size() == 0)
   {
      LinkedPointsActor::Visualize(pointIndex);
      return;
   }

   dtCore::Transformable* point = GetPointDrawable(pointIndex);
   if (point)
   {
      // First clear the old visualization data from the point.
      while (point->GetNumChildren())
      {
         point->RemoveChild(point->GetChild(0));
      }

      osg::Group* pointGroup = point->GetOSGNode()->asGroup();
      pointGroup->removeChildren(0, pointGroup->getNumChildren());

      // Always place a post directly on the point.
      osg::Vec3 postPos = GetPointPosition(pointIndex);
      dtCore::Transform prevPostTransform = PlacePost(pointIndex, 0, postPos);
      dtCore::Transform postTransform;

      // Now generate the fence posts and segments up to the next point position.
      if (pointIndex < GetPointCount() - 1)
      {
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
   if (mPostMaxDistance <= 1.0f) mPostMaxDistance = 1.0f;
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
   dtCore::Transformable* point = new dtCore::Transformable();

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

   dtCore::Transformable* point = GetPointDrawable(pointIndex);
   if (point)
   {
      std::string postMeshName = GetPostMesh(pointIndex, subIndex);
      if (!postMeshName.empty())
      {
         // Create our game mesh actor to represent the post.
         dtCore::RefPtr<dtCore::Object> postMeshObj = new dtCore::Object();
         osg::Node* postMeshNode = postMeshObj->LoadFile(postMeshName);
         postMeshObj->SetScale(osg::Vec3(mFenceScale, mFenceScale, mFenceScale));

         point->AddChild(postMeshObj.get());

         // Set the position of the object.
         postMeshObj->GetTransform(transform);
         transform.SetTranslation(position);
         postMeshObj->SetTransform(transform);
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
      dtCore::RefPtr<dtCore::Transformable> segmentTrans = NULL;

      // First try and find an existing transform.
      int childCount = point->GetNumChildren();
      for (int childIndex = 0; childIndex < childCount; childIndex++)
      {
         dtCore::DeltaDrawable* child = point->GetChild(childIndex);
         if (child && child->GetName() == "OriginTransform")
         {
            segmentTrans = dynamic_cast<dtCore::Transformable*>(child);
            break;
         }
      }

      // If the origin transform does not exist, make one.
      if (!segmentTrans.valid())
      {
        segmentTrans = new dtCore::Transformable("OriginTransform");
        point->AddChild(segmentTrans.get());
        dtCore::Transform segmentTransform;
        segmentTrans->SetTransform(segmentTransform);
      }

      // create Geometry object to store all the vertices and lines primitive.
      osg::Geometry* geom = new osg::Geometry();

      // add the points geometry to the geode.
      osg::Geode* geode = new osg::Geode();
      geode->addDrawable(geom);

      osg::Group* segmentGroup = segmentTrans->GetOSGNode()->asGroup();
      segmentGroup->addChild(geode);

      osg::ref_ptr<osg::Vec3Array> vertexList   = new osg::Vec3Array();
      osg::ref_ptr<osg::Vec2Array> textureList  = new osg::Vec2Array();
      osg::ref_ptr<osg::Vec4Array> colorList    = new osg::Vec4Array();
      osg::ref_ptr<osg::Vec3Array> normalList   = new osg::Vec3Array();

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
      vertexList->push_back(start.GetTranslation() + startWidth);
      /// Top left vertex.
      vertexList->push_back((start.GetTranslation() + startWidth) + (startUp * mSegmentHeight * mFenceScale));
      /// Top right vertex.
      vertexList->push_back((end.GetTranslation() + endWidth) + (endUp * mSegmentHeight * mFenceScale));
      /// Bottom right vertex.
      vertexList->push_back(end.GetTranslation() + endWidth);

      // Back Wall
      /// Bottom left vertex.
      vertexList->push_back(end.GetTranslation() - endWidth);
      /// Top left vertex.
      vertexList->push_back((end.GetTranslation() - endWidth) + (endUp * mSegmentHeight * mFenceScale));
      /// Top right vertex.
      vertexList->push_back((start.GetTranslation() - startWidth) + (startUp * mSegmentHeight * mFenceScale));
      /// Bottom right vertex.
      vertexList->push_back(start.GetTranslation() - startWidth);

      // Top Wall
      /// Bottom left vertex.
      vertexList->push_back((start.GetTranslation() + startWidth) + (startUp * mSegmentHeight * mFenceScale));
      /// Top left vertex.
      vertexList->push_back((start.GetTranslation() - startWidth) + (startUp * mSegmentHeight * mFenceScale));
      /// Top right vertex.
      vertexList->push_back((end.GetTranslation() - endWidth) + (endUp * mSegmentHeight * mFenceScale));
      /// Bottom right vertex.
      vertexList->push_back((end.GetTranslation() + endWidth) + (endUp * mSegmentHeight * mFenceScale));

      geom->setVertexArray(vertexList.get());

      ////////////////////////////////////////////////////////////////////////////////
      // Texture
      // Front Wall
      textureList->push_back(osg::Vec2(1.0f, 0.0f));
      textureList->push_back(osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
      textureList->push_back(osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));
      textureList->push_back(osg::Vec2(1.0f - length, 0.0f));

      // Back Wall
      textureList->push_back(osg::Vec2(1.0f - length, 0.0f));
      textureList->push_back(osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));
      textureList->push_back(osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
      textureList->push_back(osg::Vec2(1.0f, 0.0f));

      // Top Wall
      textureList->push_back(osg::Vec2(1.0f, 1.0f - mTopTextureRatio));
      textureList->push_back(osg::Vec2(1.0f, 1.0f));
      textureList->push_back(osg::Vec2(1.0f - length, 1.0f));
      textureList->push_back(osg::Vec2(1.0f - length, 1.0f - mTopTextureRatio));

      geom->setTexCoordArray(0, textureList.get());

      ////////////////////////////////////////////////////////////////////////////////
      // Color
      colorList->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      geom->setColorArray(colorList.get());
      geom->setColorBinding(osg::Geometry::BIND_OVERALL);

      ////////////////////////////////////////////////////////////////////////////////
      // Normal
      normalList->push_back(dir);
      geom->setNormalArray(normalList.get());
      geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

      // This time we simply use primitive, and hardwire the number of coords to use
      // since we know up front,
      geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
      geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 4, 4));
      geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 8, 4));

      // Get the texture to use for this segment.
      std::string textureName = GetSegmentTexture(pointIndex, subIndex);
      if (!textureName.empty())
      {
         // set up the texture state.
         osg::Texture2D* texture = new osg::Texture2D();
         texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
         dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(textureName);
         texture->setImage(osgDB::readImageFile(dtDAL::Project::GetInstance().GetResourcePath(descriptor)));

         osg::StateSet* stateset = geom->getOrCreateStateSet();
         stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
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
   SetActor(*new FenceActor());
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
