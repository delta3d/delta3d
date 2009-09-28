#include <dtActors/buildingactor.h>
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
#include <osg/CullFace>
#include <osg/PolygonMode>

namespace dtActors
{
   ////////////////////////////////////////////////////////////////////////////////
   // BUILDING GEOM DATA
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   bool BuildingGeomData::Initialize()
   {
      if (!mParent) return false;

      // Create the Wall Geometry object.
      mWallGeom[0] = new osg::Geometry();
      if (!mWallGeom[0].valid()) return false;

      mWallGeom[1] = new osg::Geometry();
      if (!mWallGeom[1].valid()) return false;

      mWallGeode = new osg::Geode();
      if (!mWallGeode.valid()) return false;

      mWallGeode->addDrawable(mWallGeom[0].get());
      mWallGeode->addDrawable(mWallGeom[1].get());

      BuildingGeomNode* buildingParent = dynamic_cast<BuildingGeomNode*>(mParent);
      if (!buildingParent) return false;

      // Add the Geode to the transformable node.
      osg::Group* originGroup = buildingParent->mOrigin->GetOSGNode()->asGroup();
      if (!originGroup) return false;
      originGroup->addChild(mWallGeode.get());

      // Create all of our buffer arrays.
      mWallVertexList = new osg::Vec3Array();
      if (!mWallVertexList.valid()) return false;
      mWallTextureList = new osg::Vec2Array();
      if (!mWallTextureList.valid()) return false;
      mWallNormalList = new osg::Vec3Array();
      if (!mWallNormalList.valid()) return false;

      // Set the size of the buffer arrays.
      mWallVertexList->resize(8);
      mWallTextureList->resize(8);
      mWallNormalList->resize(2);

      // Apply the buffer arrays to the geometry.
      mWallGeom[0]->setVertexArray(mWallVertexList.get());
      mWallGeom[1]->setVertexArray(mWallVertexList.get());

      mWallGeom[0]->setTexCoordArray(0, mWallTextureList.get());
      mWallGeom[1]->setTexCoordArray(0, mWallTextureList.get());

      mWallGeom[0]->setColorArray(buildingParent->mSegColorList.get());
      mWallGeom[0]->setColorBinding(osg::Geometry::BIND_OVERALL);
      mWallGeom[1]->setColorArray(buildingParent->mSegColorList.get());
      mWallGeom[1]->setColorBinding(osg::Geometry::BIND_OVERALL);

      mWallGeom[0]->setNormalArray(mWallNormalList.get());
      mWallGeom[0]->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
      mWallGeom[1]->setNormalArray(mWallNormalList.get());
      mWallGeom[1]->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

      // Now Set the primitive sets for the three sides of the segment.
      mWallGeom[0]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
      mWallGeom[1]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 4, 4));

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BuildingGeomData::Shutdown()
   {
      if (!mParent) return false;

      BuildingGeomNode* buildingParent = dynamic_cast<BuildingGeomNode*>(mParent);
      if (!buildingParent) return false;

      osg::Group* originGroup = buildingParent->mOrigin->GetOSGNode()->asGroup();
      if (!originGroup) return false;
      originGroup->removeChild(mWallGeode.get());

      mWallGeom[0] = NULL;
      mWallGeom[1] = NULL;
      mWallGeode = NULL;

      mWallVertexList = NULL;
      mWallTextureList = NULL;
      mWallNormalList = NULL;

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   // BUILDING GEOM NODE
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   BuildingGeomNode::BuildingGeomNode()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   BuildingGeomNode::~BuildingGeomNode()
   {
      SetSize(0);
      mOrigin = NULL;
      mSegColorList = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   LinkedPointsGeomDataBase* BuildingGeomNode::CreateGeom()
   {
      // Make sure our origin transformable exists.
      if (!mOrigin.valid())
      {
         mOrigin = new dtCore::Transformable();
         AddChild(mOrigin.get());
         dtCore::Transform segmentTransform;
         mOrigin->SetTransform(segmentTransform);
      }

      // Make sure our color list array exists.
      if (!mSegColorList.valid())
      {
         mSegColorList = new osg::Vec4Array();
         mSegColorList->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      }

      return new BuildingGeomData(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   BuildingActor::BuildingActor(dtActors::LinkedPointsActorProxy* proxy, const std::string& name)
      : BaseClass(proxy, name)
      , mRoofTextureScale(10.0f)
      , mWallTextureScale(10.0f)
      , mBuildingHeight(3.0f)
      , mGenerateRoof(false)
      , mFlatRoof(true)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   BuildingActor::~BuildingActor()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BuildingActor::Initialize()
   {
      // Create the Wall Geometry object.
      mRoofGeom = new osg::Geometry();
      mRoofGeode = new osg::Geode();

      mRoofGeode->addDrawable(mRoofGeom.get());

      // Add the Geode to the transformable node.
      mOrigin = new dtCore::Transformable();
      osg::Group* originGroup = mOrigin->GetOSGNode()->asGroup();
      originGroup->addChild(mRoofGeode.get());
      AddChild(mOrigin.get());

      // Create all of our buffer arrays.
      mRoofVertexList = new osg::Vec3Array();
      mRoofTextureList = new osg::Vec2Array();
      mRoofColorList = new osg::Vec4Array();
      mRoofNormalList = new osg::Vec3Array();

      // Apply the buffer arrays to the geometry.
      mRoofGeom->setVertexArray(mRoofVertexList.get());

      mRoofGeom->setTexCoordArray(0, mRoofTextureList.get());

      mRoofColorList->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      mRoofGeom->setColorArray(mRoofColorList.get());
      mRoofGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

      mRoofNormalList->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
      mRoofGeom->setNormalArray(mRoofNormalList.get());
      mRoofGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

      // Create a texture object to use for the roof.
      mRoofTexture = new osg::Texture2D();
      mRoofTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
      mRoofTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

      osg::StateSet* stateset = mRoofGeom->getOrCreateStateSet();
      stateset->setTextureAttributeAndModes(0, mRoofTexture.get(), osg::StateAttribute::ON);

      // Make sure we render both sides, so disable cull face.
      stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

      // Create a texture object to use for the walls.
      mOutWallTexture = new osg::Texture2D();
      mOutWallTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
      mOutWallTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

      mInWallTexture = new osg::Texture2D();
      mInWallTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
      mInWallTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

      AddPoint(osg::Vec3());
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::Visualize()
   {
      if (!mProxy->IsLoading())
      {
         mGenerateRoof = true;

         mRoofGeom->removePrimitiveSet(0, mRoofGeom->getNumPrimitiveSets());
         mRoofPoints.clear();
         mRoofVertexList->clear();
         mRoofTextureList->clear();

         dtCore::Transform transform;
         mOrigin->SetTransform(transform);

         BaseClass::Visualize();

         mGenerateRoof = false;

         // Now generate the roof of the building.
         GenerateRoof(mRoofPoints);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::Visualize(int pointIndex)
   {
      if (mProxy->IsLoading())
      {
         return;
      }

      BuildingGeomNode* point = dynamic_cast<BuildingGeomNode*>(GetPointDrawable(pointIndex));
      // If the drawable is not the proper class, regenerate it.
      if (!point)
      {
         RegeneratePointDrawable(pointIndex);

         point = dynamic_cast<BuildingGeomNode*>(GetPointDrawable(pointIndex));
      }

      if (point)
      {
         // Generate a wall between this point and the next if we can.
         if (pointIndex < GetPointCount() - 1)
         {
            int nextPoint = pointIndex + 1;

            // Get our start and end points.
            osg::Vec3 start = GetPointPosition(pointIndex);
            osg::Vec3 end = GetPointPosition(nextPoint);

            PlaceWall(pointIndex, start, end);
         }
         // The last point always connects to the first.
         else
         {
            if (GetPointCount() > 2)
            {
               int nextPoint = 0;

               // Get our start and end points.
               osg::Vec3 start = GetPointPosition(pointIndex);
               osg::Vec3 end = GetPointPosition(nextPoint);

               PlaceWall(pointIndex, start, end);
            }
            // If we don't have at least three points, we can't make a loop.
            else
            {
               point->SetSize(0);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::AddPoint(osg::Vec3 location, int index)
   {
      BaseClass::AddPoint(location, index);
      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int BuildingActor::AddPointOnSegment(osg::Vec3 location)
   {
      // Must have at least one segment (two connected points).
      if (GetPointCount() < 2)
      {
         return -1;
      }

      float nearestDistance = -1;
      int newIndex = -1;
      osg::Vec3 newPosition;

      for (int index = 0; index < GetPointCount(); index++)
      {
         osg::Vec3 firstPoint = GetPointPosition(index);

         int nextIndex = index + 1;
         if (nextIndex >= GetPointCount()) nextIndex = 0;
         osg::Vec3 secondPoint = GetPointPosition(nextIndex);

         osg::Vec3 pos = FindNearestPointOnLine(firstPoint, secondPoint, location);

         float distance = (pos - location).length2();
         if (nearestDistance == -1 || distance < nearestDistance)
         {
            nearestDistance = distance;
            newIndex = index + 1;
            newPosition = pos;
         }
      }

      if (newIndex > -1)
      {
         AddPoint(newPosition, newIndex);
      }

      return newIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::RemovePoint(int index)
   {
      BaseClass::RemovePoint(index);
      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetPointPosition(int index, osg::Vec3 location)
   {
      BaseClass::SetPointPosition(index, location);
      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetPointRotation(int index, osg::Vec3 rotation)
   {
      BaseClass::SetPointRotation(index, rotation);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int BuildingActor::GetPointIndex(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPos)
   {
      // First check if we are selecting the roof.
      if (drawable == mOrigin.get())
      {
         int closestPoint = 0;
         float closestDistance = -1.0f;

         // If we are hitting the roof, all we want is the closest point.
         for (int pointIndex = 0; pointIndex < GetPointCount(); pointIndex++)
         {
            osg::Vec3 pos = GetPointPosition(pointIndex);
            float distance = (pickPos - pos).length();

            if (closestDistance == -1.0f || distance < closestDistance)
            {
               closestDistance = distance;
               closestPoint = pointIndex;
            }
         }

         return closestPoint;
      }

      int pointIndex = BaseClass::GetPointIndex(drawable);
      if (pointIndex >= 0)
      {
         int nextPoint = pointIndex + 1;
         if (nextPoint >= GetPointCount()) nextPoint = 0;

         osg::Vec3 firstPos = GetPointPosition(pointIndex);
         osg::Vec3 secondPos = GetPointPosition(nextPoint);

         float fDistance1 = (pickPos - firstPos).length();
         float fDistance2 = (pickPos - secondPos).length();

         // If we are closer to our second point, then change our selection index.
         if (fDistance1 > fDistance2)
         {
            pointIndex = nextPoint;
         }
      }

      return pointIndex;
   }

   //////////////////////////////////////////////////////////////////////////////////
   //void BuildingActor::SetAttachedList(const std::vector<dtCore::UniqueId>& attachedList)
   //{
   //   mAttachedActors = attachedList;
   //}

   //////////////////////////////////////////////////////////////////////////////////
   //void BuildingActor::SetAttachedListPointers(const std::vector<dtCore::Transformable*>& attachedList)
   //{
   //   mAttachedActorPointers = attachedList;
   //}

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::AttachActor(dtCore::Transformable* actor)
   {
      //mAttachedActors.push_back(actor->GetUniqueId());

      // Now orient the actor to face out from the wall.
      dtCore::Transform transform;
      actor->GetTransform(transform);

      // Must have at least one segment (two connected points).
      if (GetPointCount() < 2)
      {
         return;
      }

      float nearestDistance = -1;
      int wallIndex = -1;
      osg::Vec3 wallNormal;
      osg::Vec3 newPosition;

      for (int index = 0; index < GetPointCount(); index++)
      {
         osg::Vec3 firstPoint = GetPointPosition(index);

         int nextIndex = index + 1;
         if (nextIndex >= GetPointCount()) nextIndex = 0;
         osg::Vec3 secondPoint = GetPointPosition(nextIndex);

         osg::Vec3 pos = FindNearestPointOnLine(firstPoint, secondPoint, transform.GetTranslation());

         float distance = (pos - transform.GetTranslation()).length2();
         if (nearestDistance == -1 || distance < nearestDistance)
         {
            nearestDistance = distance;
            wallIndex = index;
            newPosition = pos;

            wallNormal = secondPoint - firstPoint;
            wallNormal.normalize();
            wallNormal = osg::Vec3(0.0f, 0.0f, 1.0f) ^ wallNormal;
         }
      }

      if (wallIndex > -1)
      {
         float distance = (newPosition - transform.GetTranslation()).length2();

         // If the object is close enough to the position, then we'll consider it attached.
         if (distance < 100.0f)
         {
            if (PointInBuilding(GetPointPosition(wallIndex) + wallNormal))
            {
               transform.Set(transform.GetTranslation(), transform.GetTranslation() + wallNormal, osg::Vec3(0.0f, 0.0f, 1.0f));
            }
            else
            {
               transform.Set(transform.GetTranslation(), transform.GetTranslation() - wallNormal, osg::Vec3(0.0f, 0.0f, 1.0f));
            }

            actor->SetTransform(transform);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetRoofTexture(const std::string& fileName)
   {
      if (mRoofTexture.valid())
      {
         if (!fileName.empty())
         {
            // set up the texture state.
            dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(fileName);
            mRoofTexture->setImage(osgDB::readImageFile(dtDAL::Project::GetInstance().GetResourcePath(descriptor)));
         }
         else
         {
            mRoofTexture->setImage(NULL);
         }
         Visualize();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetOutWallTexture(const std::string& fileName)
   {
      if (mOutWallTexture.valid())
      {
         if (!fileName.empty())
         {
            // set up the texture state.
            dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(fileName);
            mOutWallTexture->setImage(osgDB::readImageFile(dtDAL::Project::GetInstance().GetResourcePath(descriptor)));
         }
         else
         {
            mOutWallTexture->setImage(NULL);
         }
         Visualize();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetInWallTexture(const std::string& fileName)
   {
      if (mInWallTexture.valid())
      {
         if (!fileName.empty())
         {
            // set up the texture state.
            dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(fileName);
            mInWallTexture->setImage(osgDB::readImageFile(dtDAL::Project::GetInstance().GetResourcePath(descriptor)));
         }
         else
         {
            mInWallTexture->setImage(NULL);
         }
         Visualize();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetRoofTextureScale(float value)
   {
      mRoofTextureScale = value;

      if (mRoofTextureScale <= 0.0f) mRoofTextureScale = 0.0f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetWallTextureScale(float value)
   {
      mWallTextureScale = value;

      if (mWallTextureScale <= 0.0f) mWallTextureScale = 0.0f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetBuildingHeight(float value)
   {
      mBuildingHeight = value;

      if (mBuildingHeight <= 0.0f) mBuildingHeight = 0.0f;

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BuildingActor::GetFlatRoofFlag()
   {
      return mFlatRoof;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetFlatRoofFlag(bool value)
   {
      if (mFlatRoof != value)
      {
         mFlatRoof = value;
         Visualize();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 BuildingActor::GetScale() const
   {
      return osg::Vec3(mRoofTextureScale, mWallTextureScale, mBuildingHeight);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::SetScale(const osg::Vec3& value)
   {
      mRoofTextureScale = value.x();
      mWallTextureScale = value.y();
      mBuildingHeight = value.z();

      Visualize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* BuildingActor::CreatePointDrawable(osg::Vec3 position)
   {
      dtCore::Transformable* point = new BuildingGeomNode();
      
      // Attach this new point to the actor.
      AddChild(point);

      // Set the position of this point.
      dtCore::Transform transform;
      point->GetTransform(transform);
      transform.SetTranslation(position);
      point->SetTransform(transform);

      return point;
   }

   ////////////////////////////////////////////////////////
   //void BuildingActor::OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   //{
   //   // Re-visualize geometry when this actor has been rotated.
   //   Visualize();

   //   dtCore::Transform transform;
   //   GetTransform(transform);
   //   osg::Vec3 center = transform.GetTranslation();

   //   osg::Vec3 diff;
   //   diff.x() = newValue.z() - oldValue.z();
   //   //diff.y() = newValue.x() - oldValue.x();
   //   //diff.z() = newValue.y() - oldValue.y();

   //   // Rotate all attached actors with the building.
   //   for (int index = 0; index < (int)mAttachedActorPointers.size(); index++)
   //   {
   //      dtCore::Transformable* actor = mAttachedActorPointers[index];
   //      if (actor)
   //      {
   //         actor->GetTransform(transform);
   //         osg::Vec3 offset = transform.GetTranslation() - center;
   //         osg::Vec3 hpr;
   //         transform.GetRotation(hpr);
   //         hpr += diff;
   //         transform.SetRotation(hpr);

   //         //dtCore::Transform rotation;
   //         //rotation.SetRotation(hpr);

   //         //osg::Vec3 newPos = (offset * rotation) + center;
   //         actor->SetTransform(transform);

   //         //osg::Matrix matrix = actor->GetMatrix();
   //         //osg::Vec3 offset = matrix.getTrans() - center;
   //         //matrix = matrix
   //         //matrix = matrix.rotate(oldValue, newValue);
   //         //actor->SetMatrix(matrix);



   //         //osg::Vec3 offset = matrix.getTrans() - center;

   //         ////matrix = rotation.rotate(oldValue, newValue);
   //         ////matrix.setTrans(0.0f, 0.0f, 0.0f);
   //         //osg::Vec3 newPos = (offset * rotation) + center;
   //         //matrix.setTrans(newPos);
   //         //actor->SetMatrix(matrix);
   //      }
   //   }
   //}

   //////////////////////////////////////////////////////////////////////////////////
   //void BuildingActor::OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   //{
   //   // Re-visualize geometry when this actor has been translated.
   //   Visualize();

   //   // Translate all attached actors with the building.
   //   osg::Vec3 offset = newValue - oldValue;
   //   for (int index = 0; index < (int)mAttachedActorPointers.size(); index++)
   //   {
   //      dtCore::Transformable* actor = mAttachedActorPointers[index];
   //      if (actor)
   //      {
   //         dtCore::Transform transform;
   //         actor->GetTransform(transform);
   //         transform.SetTranslation(transform.GetTranslation() + offset);
   //         actor->SetTransform(transform);
   //      }
   //   }
   //}

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::PlaceWall(int pointIndex, osg::Vec3 start, osg::Vec3 end)
   {
      BuildingGeomNode* point = dynamic_cast<BuildingGeomNode*>(GetPointDrawable(pointIndex));
      if (point)
      {
         if (!point->SetSize(1)) return;

         BuildingGeomData* geomData = dynamic_cast<BuildingGeomData*>(point->mGeomList[0].get());
         if (!geomData) return;

         // Always reset the origin transform to the origin of the world.
         dtCore::Transform segmentTransform;
         point->mOrigin->SetTransform(segmentTransform);

         // Get the vector between the two posts.
         osg::Vec3 dir = end - start;
         float wallLength = dir.length();

         dir.x() = end.x() - start.x();
         dir.y() = end.y() - start.y();
         dir.z() = 0.0f;
         dir.normalize();

         osg::Vec3 normal = osg::Vec3(0.0f, 0.0f, 1.0f) ^ dir;

         int nextIndex = pointIndex + 1;
         if (nextIndex >= GetPointCount()) nextIndex = 0;

         float startRoofHeight = CalculateRoofHeight(pointIndex);
         float endRoofHeight = CalculateRoofHeight(nextIndex);

         osg::Vec3 up = osg::Vec3(0.0f, 0.0f, 1.0f);
         float startHeight = startRoofHeight - start.z();
         float endHeight = endRoofHeight - end.z();

         float textureLength = wallLength * (mWallTextureScale * 0.1f);

         // If we are generating a roof, make sure we add the roof point into our list
         if (mGenerateRoof)
         {
            mRoofPoints.push_back(start + up * startHeight);
         }

         ////////////////////////////////////////////////////////////////////////////////
         // Vertex
         // Front Wall
         /// Bottom left vertex.
         geomData->mWallVertexList->at(0) = start;
         /// Top left vertex.
         geomData->mWallVertexList->at(1) = start + up * startHeight;
         /// Top right vertex.
         geomData->mWallVertexList->at(2) = end + up * endHeight;
         /// Bottom right vertex.
         geomData->mWallVertexList->at(3) = end;

         // Back Wall
         /// Bottom left vertex.
         geomData->mWallVertexList->at(4) = end;
         /// Top left vertex.
         geomData->mWallVertexList->at(5) = end + up * endHeight;
         /// Top right vertex.
         geomData->mWallVertexList->at(6) = start + up * startHeight;
         /// Bottom right vertex.
         geomData->mWallVertexList->at(7) = start;


         ////////////////////////////////////////////////////////////////////////////////
         // Texture
         // Front Wall
         geomData->mWallTextureList->at(0) = (osg::Vec2(1.0f, 1.0f - startHeight * (mWallTextureScale * 0.1f)));
         geomData->mWallTextureList->at(1) = (osg::Vec2(1.0f, 1.0f));
         geomData->mWallTextureList->at(2) = (osg::Vec2(1.0f - textureLength, 1.0f));
         geomData->mWallTextureList->at(3) = (osg::Vec2(1.0f - textureLength, 1.0f - endHeight * (mWallTextureScale * 0.1f)));

         // Back Wall
         geomData->mWallTextureList->at(4) = (osg::Vec2(1.0f - textureLength, 1.0f - endHeight * (mWallTextureScale * 0.1f)));
         geomData->mWallTextureList->at(5) = (osg::Vec2(1.0f - textureLength, 1.0f));
         geomData->mWallTextureList->at(6) = (osg::Vec2(1.0f, 1.0f));
         geomData->mWallTextureList->at(7) = (osg::Vec2(1.0f, 1.0f - startHeight * (mWallTextureScale * 0.1f)));


         ////////////////////////////////////////////////////////////////////////////////
         // Normal
         geomData->mWallNormalList->at(0) = normal;
         geomData->mWallNormalList->at(1) = -normal;

         int inWallIndex = 0;
         osg::Vec3 midPoint = start + ((end - start) * 0.5f);
         if (PointInBuilding(midPoint + normal))
         {
            inWallIndex = 1;
         }

         osg::StateSet* stateset = geomData->mWallGeom[inWallIndex]->getOrCreateStateSet();
         if (stateset && mOutWallTexture.valid())
         {
            stateset->setTextureAttributeAndModes(0, mOutWallTexture.get(), osg::StateAttribute::ON);
         }

         stateset = geomData->mWallGeom[inWallIndex ^ 1]->getOrCreateStateSet();
         if (stateset && mInWallTexture.valid())
         {
            stateset->setTextureAttributeAndModes(0, mInWallTexture.get(), osg::StateAttribute::ON);
         }

         // Make sure the geometry node knows to re-calculate the bounding area and display lists.
         geomData->mWallGeom[0]->dirtyBound();
         geomData->mWallGeom[0]->dirtyDisplayList();
         geomData->mWallGeom[1]->dirtyBound();
         geomData->mWallGeom[1]->dirtyDisplayList();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   float BuildingActor::CalculateRoofHeight(int pointIndex)
   {
      float highestValue = 0;
      if (mFlatRoof)
      {
         // First find the highest point.
         for (int index = 0; index < GetPointCount(); index++)
         {
            osg::Vec3 position = GetPointPosition(index);

            if (index == 0 || position.z() > highestValue)
            {
               highestValue = position.z();
            }
         }
      }
      else
      {
         osg::Vec3 position = GetPointPosition(pointIndex);
         highestValue = position.z();
      }

      return highestValue + mBuildingHeight;
   }

   ////////////////////////////////////////////////////////////////////////////////
   BuildingActor::RoofReturnData BuildingActor::GenerateRoof(std::vector<osg::Vec3> roofPoints, int masterIndex, int stackCount)
   {
      BuildingActor::RoofReturnData returnData;
      returnData.bFailed = false;
      returnData.firstFailedIndex = -1;
      returnData.endFailedIndex = -1;

      int roofPointSize = (int)roofPoints.size();
      // If we don't have enough points, we can't generate a roof.
      if (roofPointSize < 3)
      {
         returnData.bFailed = true;
         return returnData;
      }

      // Return if we have iterated too deep into the stack.
      if (stackCount > 50)
      {
         returnData.bFailed = true;
         return returnData;
      }

      // If we have iterated our master index through the entire point list, then render it.
      if (masterIndex >= roofPointSize)
      {
         DrawRoof(roofPoints);
         return returnData;
      }

      osg::Vec3 intersectionPoint;
      std::vector<int> passedPoints;

      int firstFailedIndex = -1;
      int endFailedIndex = -1;

      // Iterate through each point until we find a point that is not
      // within line of sight of the first.
      osg::Vec3 masterPoint = roofPoints[0];

      // The first point is the master point, so it is valid.
      passedPoints.push_back(0);

      // We start the iteration at the 3'rd point because the second
      // Will always be adjacent to the master point and therefore
      // a valid line of sight.
      int lastPointIndex = 1;
      for (int pointIndex = 1; pointIndex < (int)roofPoints.size(); pointIndex++, lastPointIndex++)
      {
         bool bHasLOS = true;

         osg::Vec3 testPoint = roofPoints[pointIndex];

         // Now perform a line of sight collision test between the two points
         // against all other lines of the roof.
         for (int testIndex = 0; testIndex < (int)mRoofPoints.size(); testIndex++)
         {
            osg::Vec3 firstPoint = mRoofPoints[testIndex];
            osg::Vec3 secondPoint;
            
            if (testIndex < (int)mRoofPoints.size() - 1)
            {
               secondPoint = mRoofPoints[testIndex + 1];
            }
            else
            {
               secondPoint = mRoofPoints[0];
            }

            if (IntersectionTest(masterPoint, testPoint, firstPoint, secondPoint, intersectionPoint))
            {
               // If any of the tests fail, we are finished and this test point is
               // has failed its line of sight test.
               bHasLOS = false;
               break;
            }
         }

         // If we have line of sight to the test point, it is a valid point.
         if (bHasLOS)
         {
            passedPoints.push_back(pointIndex);
         }
         // If we do not have line of sight, we are finished with this iteration.
         else
         {
            // Put our test point into the failure list.
            firstFailedIndex = endFailedIndex = pointIndex;
            break;
         }
      }

      // Now iterate the same test the other direction.
      int insertIndex = -1;
      for (int pointIndex = (int)roofPoints.size() - 1; pointIndex > lastPointIndex; pointIndex--)
      {
         bool bHasLOS = true;

         osg::Vec3 testPoint = roofPoints[pointIndex];

         // Now perform a line of sight collision test between the two points
         // against all other lines of the roof.
         for (int testIndex = 0; testIndex < (int)mRoofPoints.size(); testIndex++)
         {
            osg::Vec3 firstPoint = mRoofPoints[testIndex];
            osg::Vec3 secondPoint;

            if (testIndex < (int)mRoofPoints.size() - 1)
            {
               secondPoint = mRoofPoints[testIndex + 1];
            }
            else
            {
               secondPoint = mRoofPoints[0];
            }

            if (IntersectionTest(masterPoint, testPoint, firstPoint, secondPoint, intersectionPoint))
            {
               // If any of the tests fail, we are finished and this test point is
               // has failed its line of sight test.
               bHasLOS = false;
               break;
            }
         }

         // If we have line of sight to the test point, it is a valid point.
         if (bHasLOS)
         {
            // The points must be placed in this list in order of left to right
            // so the triangle fan can be properly generated.
            if (insertIndex == -1)
            {
               insertIndex = (int)passedPoints.size();
               passedPoints.push_back(pointIndex);
            }
            else
            {
               passedPoints.insert(passedPoints.begin() + insertIndex, pointIndex);
            }
         }
         // If we do not have line of sight, we are finished with this iteration.
         else
         {
            // Put our test point into the failure list.
            endFailedIndex = pointIndex;
            break;
         }
      }

      // Now we need to change the master point and double check our passed points list.
      std::vector<osg::Vec3> points;
      for (int pointIndex = 0; pointIndex < (int)passedPoints.size(); pointIndex++)
      {
         points.push_back(roofPoints[passedPoints[pointIndex]]);
      }

      // Bail out if we don't even have enough points to render a triangle.
      if (points.size() < 3)
      {
         // -1 means we couldn't generate any triangles with these points.
         returnData.bFailed = true;
         return returnData;
      }

      // Now shift the points over so the master index is the next point.
      masterPoint = points[0];
      points.erase(points.begin());
      points.push_back(masterPoint);
      BuildingActor::RoofReturnData returnedData = GenerateRoof(points, masterIndex + 1, stackCount + 1);
      if (returnedData.bFailed && returnedData.firstFailedIndex > -1)
      {
         //for (int failedIndex = returnedData.firstFailedIndex + 1;
         //   failedIndex <= returnedData.endFailedIndex + 1;
         //   failedIndex++)
         //{

         //}
         if (returnedData.firstFailedIndex + 1 < (int)passedPoints.size())
         {
            int firstFailed = passedPoints[returnedData.firstFailedIndex + 1];
            if (firstFailed < firstFailedIndex)
            {
               firstFailedIndex = firstFailed;
            }
         }
         if (returnedData.endFailedIndex + 1 < (int)passedPoints.size())
         {
            int endFailed = passedPoints[returnedData.endFailedIndex + 1];
            if (endFailed > endFailedIndex)
            {
               endFailedIndex = endFailed;
            }
         }
      }

      // If we came across a failed point, then we need to generate another section of the roof.
      if (firstFailedIndex > -1 && endFailedIndex > -1)
      {
         // now generate a new roof point list with completed roof portions excluded.
         std::vector<osg::Vec3> newRoofPoints;

         for (int pointIndex = firstFailedIndex - 1; pointIndex <= endFailedIndex + 1; pointIndex++)
         {
            int actualPointIndex = pointIndex;
            if (actualPointIndex >= (int)roofPoints.size()) actualPointIndex -= (int)roofPoints.size();
            newRoofPoints.push_back(roofPoints[actualPointIndex]);
         }

         // Now recurse this algorithm to draw the rest of the roof.
         returnedData = GenerateRoof(newRoofPoints, 0, stackCount + 1);
         if (returnedData.bFailed && returnedData.firstFailedIndex == -1)
         {
            // If we failed to generate any geometry, return the index that failed.
            returnData.bFailed = true;
            returnData.firstFailedIndex = firstFailedIndex;
            returnData.endFailedIndex = endFailedIndex;
            return returnData;
         }
      }

      return returnData;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::DrawRoof(std::vector<osg::Vec3> roofPoints, int recurseCount)
   {
      // If we have recursed passed the limit of our point size, there is nothing
      // we can do, so don't draw anything.
      if (recurseCount > (int)roofPoints.size())
      {
         DrawRoofPortion(roofPoints);
         return;
      }

      std::vector<osg::Vec3> points;

      // Our master point is always our TRIANGLE_FAN base.
      osg::Vec3 masterPoint = roofPoints[0];
      points.push_back(masterPoint);

      // Our adjacent point is always the point directly adjacent to the master point in the negative direction.
      osg::Vec3 adjacentPoint = roofPoints[(int)roofPoints.size() - 1];

      // Iterate through the roof triangle.
      for (int pointIndex = 1; pointIndex < (int)roofPoints.size() - 1; pointIndex++)
      {
         // Get the first and second points that will connect with our master point to make up our triangle.
         osg::Vec3 p1 = roofPoints[pointIndex];
         osg::Vec3 p2 = roofPoints[pointIndex + 1];
         osg::Vec3 p3 = adjacentPoint;

         // Make our points relative to the master point.
         p1 -= masterPoint;
         p2 -= masterPoint;
         p3 -= masterPoint;

         p1.normalize();
         p2.normalize();
         p3.normalize();

         // The direction vector is the normal if master point to point 1.
         osg::Vec3 dirVec = p1 ^ osg::Vec3(0.0f, 0.0f, 1.0f);

         // If p2 and p3 are not in the same side of p1, then we have an overhanging roof.
         float p2Dir = dirVec * p2;
         float p3Dir = dirVec * p3;

         // The test angle is the angle of the master point between our two desired triangle points.
         float testAngle   = p1 * p2;
         
         // The actual angle is the angle of the master point between its two adjacent points.
         float actualAngle = p1 * p3;

         // If our angles are on opposite sides or the test angle is
         // smaller than the actual angle, it means we have an
         // overhanging roof.
         if ((p2Dir > 0.0f && p3Dir < 0.0f) ||
            (p2Dir < 0.0f && p3Dir > 0.0f) ||
            fabs(testAngle) < fabs(actualAngle))
         {
            // If we had some parts pass, draw them first.
            if (pointIndex > 1)
            {
               points.push_back(roofPoints[pointIndex]);

               DrawRoofPortion(points);

               // Remove the already drawn portion of the roof from the list.
               std::vector<osg::Vec3> newPoints;
               newPoints.push_back(roofPoints[0]);
               for (int index = pointIndex; index < (int)roofPoints.size(); index++)
               {
                  newPoints.push_back(roofPoints[index]);
               }
               DrawRoof(newPoints);
               return;
            }

            // In this case, it is necessary to change our master point and try again.
            roofPoints.erase(roofPoints.begin());
            roofPoints.push_back(masterPoint);
            DrawRoof(roofPoints, recurseCount + 1);
            return;
         }

         points.push_back(roofPoints[pointIndex]);
      }

      // If we get here, it means we have a valid roof to draw...
      DrawRoofPortion(roofPoints);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActor::DrawRoofPortion(std::vector<osg::Vec3> roofPoints)
   {
      // The first index is always the first point added.
      int firstIndex = (int)mRoofVertexList->size();

      // Add the roof points to the point list.
      for(int pointIndex = 0; pointIndex < (int)roofPoints.size(); pointIndex++)
      {
         osg::Vec3 pos = roofPoints[pointIndex];

         mRoofVertexList->push_back(pos);
         mRoofTextureList->push_back(osg::Vec2(pos.x() * (mRoofTextureScale * 0.1f), pos.y() * (mRoofTextureScale * 0.1f)));
      }

      mRoofGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, firstIndex, (int)roofPoints.size()));

      // Make sure the geometry node knows to re-calculate the bounding area and display lists.
      mRoofGeom->dirtyBound();
      mRoofGeom->dirtyDisplayList();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BuildingActor::IntersectionTest(osg::Vec3 a1, osg::Vec3 a2, osg::Vec3 b1, osg::Vec3 b2, osg::Vec3& collisionPoint)
   {
      osg::Vec3 originalStart = a1;

      // First do a basic bounding box collision test.
      osg::Vec3 minA, maxA, minB, maxB;

      if (a1.x() <= a2.x())
      {
         minA.x() = a1.x();
         maxA.x() = a2.x();
      }
      else
      {
         minA.x() = a2.x();
         maxA.x() = a1.x();
      }

      if (a1.y() <= a2.y())
      {
         minA.y() = a1.y();
         maxA.y() = a2.y();
      }
      else
      {
         minA.y() = a2.y();
         maxA.y() = a1.y();
      }

      if (b1.x() <= b2.x())
      {
         minB.x() = b1.x();
         maxB.x() = b2.x();
      }
      else
      {
         minB.x() = b2.x();
         maxB.x() = b1.x();
      }

      if (b1.y() <= b2.y())
      {
         minB.y() = b1.y();
         maxB.y() = b2.y();
      }
      else
      {
         minB.y() = b2.y();
         maxB.y() = b1.y();
      }

      if (minA.x() < maxB.x() && maxA.x() > minB.x() &&
         minA.y() < maxB.y() && maxA.y() > minB.y())
      {
         // If the two lines share an end point, we don't count that.
         if (a1 == b1 || a1 == b2 || a2 == b1 || a2 == b2)
         {
            return false;
         }

         float distA, theCos, theSin, newX, APos;

         // Translate the points so that point 1 of the first line is the origin.
         a2 -= a1;
         b1 -= a1;
         b2 -= a1;
         a1 = osg::Vec3();

         distA = (a2 - a1).length();

         // Rotate the lines so that point 2 of line 1 is on the positive X axis.
         theCos = a2.x() / distA;
         theSin = a2.y() / distA;
         newX = b1.x() * theCos + b1.y() * theSin;
         b1.y() = b1.y() * theCos - b1.x() * theSin;
         b1.x() = newX;
         newX = b2.x() * theCos + b2.y() * theSin;
         b2.y() = b2.y() * theCos - b2.x() * theSin;
         b2.x() = newX;

         // Fail if line 2 doesn't cross line 1.
         if ((b1.y() < 0.0f && b2.y() < 0.0f) ||
            (b1.y() >= 0.0f && b2.y() >= 0.0f))
         {
            return false;
         }

         // Find the intersection point along the first line.
         APos = b2.x() + (b1.x() - b2.x()) * b2.y() / (b2.y() - b1.y());

         // Fail if line 2 crosses line 1 outside of the line size.
         if (APos < 0.0f || APos > distA) return false;

         // Get the actual point of collision.
         osg::Vec3 normal = a2 - a1;
         normal.normalize();
         collisionPoint = originalStart + (normal * APos);
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BuildingActor::PointInBuilding(osg::Vec3 point)
   {
      std::vector<float> intersectPoints;

      float yVal = point.y();

      // Do a collision test between each wall and our imaginary Y axis line.
      for (int pointIndex = 0; pointIndex < GetPointCount(); pointIndex++)
      {
         int nextPoint = pointIndex + 1;
         if (nextPoint >= GetPointCount()) nextPoint = 0;

         osg::Vec3 start = GetPointPosition(pointIndex);
         osg::Vec3 end = GetPointPosition(nextPoint);

         // Create our imaginary Y axis line.
         osg::Vec3 yLineStart, yLineEnd;
         yLineStart.y() = yVal;
         yLineEnd.y() = yVal;
         yLineStart.x() = start.x();
         yLineEnd.x() = end.x();

         // If our wall line is parallel to our y axis, we can ignore it.
         if (start.y() == end.y())
         {
            continue;
         }

         // Perform a collision test.
         osg::Vec3 intersectionPoint;
         if (IntersectionTest(start, end, yLineStart, yLineEnd, intersectionPoint))
         {
            // If the point of intersection lies directly on an end point of the line,
            // only allow it if the other point is below the threshold.
            if (intersectionPoint.x() == end.x())
            {
               if (start.y() >= yVal)
               {
                  continue;
               }
            }
            else if (intersectionPoint.x() == start.x())
            {
               if (end.y() >= yVal)
               {
                  continue;
               }
            }

            // Now insert our new collision point into the sorted list.
            bool bInserted = false;
            for (int index = 0; index < (int)intersectPoints.size(); index++)
            {
               if (intersectPoints[index] >= intersectionPoint.x())
               {
                  intersectPoints.insert(intersectPoints.begin() + index, intersectionPoint.x());
                  bInserted = true;
                  break;
               }
            }

            if (!bInserted)
            {
               intersectPoints.push_back(intersectionPoint.x());
            }
         }
      }

      // Now determine if our point is within the building.
      for (int index = 0; index < (int)intersectPoints.size(); index++)
      {
         float xVal = intersectPoints[index];
         if (xVal >= point.x())
         {
            int leftCount = index;
            int rightCount = (int)intersectPoints.size() - index;

            float oddTest = float(leftCount) * 0.5f;

            // Odd numbers mean the point is inside the building.
            if (oddTest != floor(oddTest))
            {
               return true;
            }
            return false;
         }
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   BuildingActorProxy::BuildingActorProxy()
      : BaseClass()
      //, mAttachedActorIndex(0)
   {
      SetClassName("dtActors::BuildingActor");
   }

   /////////////////////////////////////////////////////////////////////////////
   BuildingActorProxy::~BuildingActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void BuildingActorProxy::CreateActor()
   {
      LinkedPointsActor* actor = new BuildingActor(this);
      SetActor(*actor);
      actor->Initialize();
   }

   /////////////////////////////////////////////////////////////////////////////
   void BuildingActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      BuildingActor* actor = NULL;
      GetActor(actor);

      //// Attached Actors.
      //dtDAL::ActorIDActorProperty* attachedActorProp = new dtDAL::ActorIDActorProperty(
      //   *this, "AttachedActor", "Attached Actor",
      //   dtDAL::ActorIDActorProperty::SetFuncType(this, &BuildingActorProxy::SetAttachedActor),
      //   dtDAL::ActorIDActorProperty::GetFuncType(this, &BuildingActorProxy::GetAttachedActor),
      //   "", "An attached actor.", "Internal");

      //dtDAL::ArrayActorPropertyBase* attachedActorArrayProp = new dtDAL::ArrayActorProperty<dtCore::UniqueId>(
      //   "AttachedActors", "Attached Actors", "The list actors that are attached.",
      //   dtDAL::MakeFunctor(*this, &BuildingActorProxy::SetAttachedActorIndex),
      //   dtDAL::MakeFunctorRet(*this, &BuildingActorProxy::GetDefaultAttachedActor),
      //   dtDAL::MakeFunctorRet(*actor, &BuildingActor::GetAttachedList),
      //   dtDAL::MakeFunctor(*actor, &BuildingActor::SetAttachedList),
      //   attachedActorProp, "Internal");
      //AddProperty(attachedActorArrayProp);

      // Roof Texture.
      dtDAL::ResourceActorProperty* roofTextureProp =
         new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
         "RoofTextureResource", "Roof Texture",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetRoofTexture),
         "Defines the texture used when rendering the roof.", "Building");
      AddProperty(roofTextureProp);

      // Outside Wall Texture.
      dtDAL::ResourceActorProperty* outWallTextureProp =
         new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
         "OutWallTextureResource", "Outside Wall Texture",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetOutWallTexture),
         "Defines the texture used when rendering walls.", "Building");
      AddProperty(outWallTextureProp);

      // Inside Wall Texture.
      dtDAL::ResourceActorProperty* inWallTextureProp =
         new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TEXTURE,
         "InWallTextureResource", "Inside Wall Texture",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetInWallTexture),
         "Defines the texture used when rendering walls.", "Building");
      AddProperty(inWallTextureProp);

      // Flat Roof.
      dtDAL::BooleanActorProperty* flatRoofProp =
         new dtDAL::BooleanActorProperty(
         "FlatRoof", "Flat Roof",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetFlatRoofFlag),
         dtDAL::MakeFunctorRet(*actor, &BuildingActor::GetFlatRoofFlag),
         "Flattens the top of the roof.", "Building");
      AddProperty(flatRoofProp);

      // Roof Texture Scale.
      dtDAL::FloatActorProperty* roofTextureScaleProp =
         new dtDAL::FloatActorProperty(
         "RoofTextureScale", "Roof Texture Scale",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetRoofTextureScale),
         dtDAL::MakeFunctorRet(*actor, &BuildingActor::GetRoofTextureScale),
         "Sets the scale of the roof texture on the building.", "Building");
      AddProperty(roofTextureScaleProp);

      // Wall Texture Scale.
      dtDAL::FloatActorProperty* wallTextureScaleProp =
         new dtDAL::FloatActorProperty(
         "WallTextureScale", "Wall Texture Scale",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetWallTextureScale),
         dtDAL::MakeFunctorRet(*actor, &BuildingActor::GetWallTextureScale),
         "Sets the scale of the wall texture on the building.", "Building");
      AddProperty(wallTextureScaleProp);

      // building height.
      dtDAL::FloatActorProperty* buildingHeightProp =
         new dtDAL::FloatActorProperty(
         "BuildingHeight", "Building Height",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetBuildingHeight),
         dtDAL::MakeFunctorRet(*actor, &BuildingActor::GetBuildingHeight),
         "Sets the height of the building.", "Building");
      AddProperty(buildingHeightProp);

      // Scale.
      dtDAL::Vec3ActorProperty* scaleProp =
         new dtDAL::Vec3ActorProperty(
         "Scale", "Scale",
         dtDAL::MakeFunctor(*actor, &BuildingActor::SetScale),
         dtDAL::MakeFunctorRet(*actor, &BuildingActor::GetScale),
         "Sets the roof texture scale (x), wall texture scale (y), and building height (z).",
         "Internal");
      AddProperty(scaleProp);
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProperty> BuildingActorProxy::GetDeprecatedProperty(const std::string& name)
   {
      dtCore::RefPtr<dtDAL::ActorProperty> prop = BaseClass::GetDeprecatedProperty(name);

      if (!prop.valid())
      {
            // Outside Wall Texture.
         if (name == "WallTextureResource")
         {
            // We have renamed this property, so redirect it.
            return GetProperty("OutWallTextureResource");
         }
      }

      return prop;
   }

   //////////////////////////////////////////////////////
   void BuildingActorProxy::OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      BaseClass::OnRotation(oldValue, newValue);

      // Re-visualize geometry when this actor has been rotated.
      BuildingActor* actor = NULL;
      GetActor(actor);
      actor->Visualize();
      //actor->OnRotation(oldValue, newValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BuildingActorProxy::OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      BaseClass::OnTranslation(oldValue, newValue);

      // Re-visualize geometry when this actor has been translated.
      BuildingActor* actor = NULL;
      GetActor(actor);
      actor->Visualize();
      //actor->OnTranslation(oldValue, newValue);
   }

   //////////////////////////////////////////////////////////////////////////////////
   //void BuildingActorProxy::SetAttachedActorIndex(int index)
   //{
   //   mAttachedActorIndex = index;
   //}

   //////////////////////////////////////////////////////////////////////////////////
   //dtCore::UniqueId BuildingActorProxy::GetDefaultAttachedActor()
   //{
   //   return dtCore::UniqueId();
   //}

   //////////////////////////////////////////////////////////////////////////////////
   //void BuildingActorProxy::SetAttachedActor(dtCore::UniqueId value)
   //{
   //   BuildingActor* actor = NULL;
   //   GetActor(actor);

   //   std::vector<dtCore::UniqueId> attachedActors = actor->GetAttachedList();
   //   if (mAttachedActorIndex < (int)attachedActors.size())
   //   {
   //      attachedActors[mAttachedActorIndex] = value;
   //      actor->SetAttachedList(attachedActors);
   //   }
   //}

   //////////////////////////////////////////////////////////////////////////////////
   //dtCore::UniqueId BuildingActorProxy::GetAttachedActor()
   //{
   //   BuildingActor* actor = NULL;
   //   GetActor(actor);

   //   std::vector<dtCore::UniqueId> attachedActors = actor->GetAttachedList();
   //   if (mAttachedActorIndex < (int)attachedActors.size())
   //   {
   //      return attachedActors[mAttachedActorIndex];
   //   }

   //   return GetDefaultAttachedActor();
   //}
}
////////////////////////////////////////////////////////////////////////////////
